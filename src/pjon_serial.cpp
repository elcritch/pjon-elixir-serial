#include "pjon_serial.h"


#include <PJON.h>

#include "erl_comm.hpp"
#include "concurrentqueue.hpp"


// std::mutex packet_next_lock;
// ErlCommsPacketTx packet_next;
moodycamel::ConcurrentQueue<ErlCommsPacketTx> tx_via_pjon;
moodycamel::ConcurrentQueue<std::string> tx_via_erl_comms;

PJON<ThroughSerial> bus(BUS_ADDR);

#include "pjon_msgpack.cpp"

void receive_erl_comms_packet() {
  char buffer[BUFFER_MAX];
  pk_len_t cmd_sz = read_port_cmd<pk_len_t>( buffer, BUFFER_MAX);

  if (cmd_sz == 0) {
    std::cerr << "STDIN closed, exiting. " << std::endl;
    exit(0);
  }

  handle_erl_comms_command(std::string(buffer, cmd_sz));
  usleep(100);
}

void transmit_erl_comms_packet() {
  std::string next_packet;
  if (tx_via_erl_comms.try_dequeue(next_packet)) {
    std::string buf;
    write_port_cmd<pk_len_t>( (char*)buf.c_str(), buf.length());
  }
}

void pjon_receiver_function(uint8_t *payload,
                       uint16_t length,
                       const PJON_Packet_Info &packet_info)
{
  std::string packed = pack_erl_comms_send_cmd(payload, length, packet_info);
  tx_via_erl_comms.enqueue(packed);
}

void pjon_error_handler(uint8_t code,
                   uint16_t data,
                   void *custom_pointer)
{
  std::string packed = erl_comms_error_msg(code, data);
  write_port_cmd<pk_len_t>( (char*)packed.c_str(), packed.length());
}

void transmit_pjon_packet() {
  ErlCommsPacketTx packet_next;
  if (tx_via_pjon.try_dequeue(packet_next)) {

    message_t m = packet_next.message;
    int result;
    int addr = packet_next.addr;

    if (packet_next.blocking) {
      result = bus.send_packet_blocking(addr, (char *) m.data(), m.size());
    } else {
      result = bus.send(addr, (char *) m.data(), m.size());
    }
  }
}

int main(int argc, char const *argv[]) {
  try {
    const char *device = argv[1];
    int baud_rate = std::stoi(argv[2]);

    #ifdef DEBUG_MODE // useful for debugging
      std::ofstream out(DEBUG_LOGFILE);
      std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
      std::cerr.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
    #endif

    std::cerr << "Setting serial... file: " << device << std::endl;
    std::cerr << "Setting serial... baud: " << baud_rate << std::endl;

    // bus.strategy.set_serial(&serial);
    int sd = serialOpen(device, baud_rate);
    if(int(sd) < 0) {
      std::cerr << "Serial open fail!" << std::endl;
      exit(1);
    }
    bus.strategy.set_serial(sd);

    // bus.strategy.set_baud_rate(baud_rate);
    bus.set_receiver(pjon_receiver_function);
    bus.set_error(pjon_error_handler);

    std::cerr << "Opening bus" << std::endl;

    bus.begin();
    std::cerr << "Success, starting communication" << std::endl;

    // Thread to handle reading input port commands
    std::thread([&]{
      try {
          while (true) {
            receive_erl_comms_packet();
          }
      } catch (const std::exception &exc) {
          std::cerr << exc.what();
          exit(11);
      }
    }).detach();

    std::thread([&]{
      try {
        while (true) {
          transmit_erl_comms_packet();
        }
      } catch (const std::exception &exc) {
        std::cerr << exc.what();
        exit(12);
      }
    }).detach();

      do {
        bus.receive(PJON_RX_WAIT_TIME);
        bus.update();
        transmit_pjon_packet();
      } while (true);

    std::cerr << "exiting..." << std::endl;
    exit(0);
  } catch (const std::exception &exc) {
    std::cerr << exc.what();
    exit(1);
  }
};

