#include "pjon_serial.h"


#include <PJON.h>

#include "erl_comm.hpp"

std::mutex packet_next_lock;
ErlCommsPacketTx packet_next;

PJON<ThroughSerial> bus(BUS_ADDR);

#include "pjon_msgpack.cpp"

void receiver_function(uint8_t *payload,
                       uint16_t length,
                       const PJON_Packet_Info &packet_info)
{
  std::string packed = pack_erl_comms_send_cmd(payload, length, packet_info);
  write_port_cmd<pk_len_t>( (char*)packed.c_str(), packed.length());
}

void error_handler(uint8_t code,
                   uint16_t data,
                   void *custom_pointer)
{
  std::string packed = erl_comms_error_msg(code, data);
  write_port_cmd<pk_len_t>( (char*)packed.c_str(), packed.length());
}

void transmitter_function() {
  if (packet_next_lock.try_lock()) {

    message_t m = packet_next.message;
    int result;
    int addr = packet_next.addr;

    if (packet_next.blocking) {
      result = bus.send_packet_blocking(addr, (char *) m.data(), m.size());
    } else {
      result = bus.send(addr, (char *) m.data(), m.size());
    }

    // What happens if an exception is thrown above?
    packet_next_lock.unlock();
  }
}

void read_erl_comms() {
  if (packet_next_lock.try_lock()) {
    handle_erl_comms_command();
    packet_next_lock.unlock();
  }
  usleep(100);
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
      try {
          while (true) {
            read_erl_comms();
          }
      } catch (const std::exception &exc) {
          std::cerr << exc.what();
      }
    }).detach();

      do {
        bus.receive(PJON_RX_WAIT_TIME);
        bus.update();
        transmitter_function();
      } while (true);

    std::cerr << "exiting..." << std::endl;
    exit(0);
  } catch (const std::exception &exc) {
    std::cerr << exc.what();
    exit(1);
  }
};

