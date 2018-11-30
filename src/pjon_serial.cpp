#include "pjon_serial.h"


#include <PJON.h>

#include "erl_comm.hpp"

std::atomic<size_t> port_rx_len;
char port_rx_buffer[BUFFER_SIZE];

PJON<ThroughSerial> bus(BUS_ADDR);

void receiver_function(uint8_t *payload,
                       uint16_t length,
                       const PJON_Packet_Info &packet_info)
{
  std::vector<unsigned char> message(payload, payload + length );

  ErlCommsPacketRx cmd_body(message, packet_info);

  auto comms_pkt = erl_comms_packet(ReceivedPjonPacket, cmd_body);

  // serialize the object into the buffer.
  std::stringstream buffer;
  msgpack::pack(buffer, comms_pkt);

  // deserialize the buffer into msgpack::object instance.
  buffer.seekg(0);
  std::string packed(buffer.str());

  write_port_cmd<pk_len_t>( (char*)packed.c_str(), packed.length());
}

void error_handler(uint8_t code,
                   uint16_t data,
                   void *custom_pointer)
{
  std::stringstream err_msg_ss;

  if(code == PJON_CONNECTION_LOST) {
    err_msg_ss << "Connection with device ID "
               << bus.packets[data].content[0]
               << " is lost.";
  }
  if(code == PJON_PACKETS_BUFFER_FULL) {
    err_msg_ss << "Packet buffer is full, has now a length of "
               << data
               << "Possible wrong bus configuration!"
               << "higher PJON_MAX_PACKETS if necessary.";
  }
  if(code == PJON_CONTENT_TOO_LONG) {
    err_msg_ss << "Content is too long, length: "
               << data;
  }

  // Well, if it works -- better way to convert to string<uint8_t> ?
  std::string tmpstr(err_msg_ss.str());
  const char *err_buffer = tmpstr.c_str();
  std::vector<char> message( err_buffer, err_buffer + tmpstr.length());

  ErlCommsPacketError cmd_body(code, message);

  auto comms_pkt = erl_comms_packet(ReceivedPjonPacket, cmd_body);

  // serialize the object into the buffer.
  std::stringstream buffer;
  msgpack::pack(buffer, comms_pkt);

  // deserialize the buffer into msgpack::object instance.
  buffer.seekg(0);
  std::string packed(buffer.str());

  write_port_cmd<pk_len_t>( (char*)packed.c_str(), packed.length());
}

void transmitter_function(std::string buffer) {
}

int main(int argc, char const *argv[]) {
  const char *device = argv[1];
  int baud_rate = std::stoi(argv[2]);

  std::cerr << "Setting serial... file: " << device << std::endl;
  std::cerr << "Setting serial... baud: " << baud_rate << std::endl;

  // bus.strategy.set_serial(&serial);
  int s = serialOpen(device, baud_rate);
  if(int(s) < 0) {
    std::cerr << "Serial open fail!" << std::endl;
    exit(1);
  }
  bus.strategy.set_serial(s);

  // bus.strategy.set_baud_rate(baud_rate);
  bus.set_receiver(receiver_function);
  bus.set_error(error_handler);

  std::cerr << "Opening bus" << std::endl;

  bus.begin();
  std::cerr << "Success, starting communication" << std::endl;

  // Thread to handle reading input port commands
  std::thread([&]{
    port_rx_len = 0;
    while (true) {
      if (port_rx_len.load() == 0) {
        // std::cerr << "port port_command reading... " << std::endl;
        pk_len_t cmd_sz =
          read_port_cmd<pk_len_t>( port_rx_buffer, PJON_PACKET_MAX_LENGTH);
        port_rx_len = cmd_sz;
        if (cmd_sz == 0) {
          std::cerr << "STDIN closed, exiting. " << std::endl;
          exit(0);
        }
      }
      usleep(100);
    }
  }).detach();

  do {
    bus.receive(PJON_RX_WAIT_TIME);

    bus.update();
    if (port_rx_len.load() > 0) {
      int rx_len = port_rx_len.load();
      #ifdef PJON_SEND_BLOCKING
        int resp = bus.send_packet_blocking(TX_PACKET_ADDR, port_rx_buffer, rx_len );
      #else
        int resp = bus.send_packet(TX_PACKET_ADDR, port_rx_buffer, rx_len);
      #endif

      port_rx_len = 0;
    }
  } while (true);

  std::cerr << "exiting..." << std::endl;
  exit(0);
};

