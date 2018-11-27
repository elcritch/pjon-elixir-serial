#include "pjon_serial.h"

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <atomic>
#include <mutex>

std::atomic<size_t> port_rx_len;
char port_rx_buffer[BUFFER_SIZE];

void receiver_function(uint8_t *payload,
                       uint16_t length,
                       const PJON_Packet_Info &packet_info)
{
  // std::cerr << "BUS rx'ed packet length: " << length << std::endl;
  write_port_cmd<pk_len_t>( (char*)payload, length);
}

void error_handler(uint8_t code,
                   uint16_t data,
                   void *custom_pointer)
{
  if (code == PJON_CONNECTION_LOST) {
    std::cerr << "error: pjon connection lost" << std::endl;
  }
  else {
    std::cerr << "error: pjon packet failure" << std::endl;
  }
}

int main(int argc, char const *argv[]) {
  const char *device = argv[1];
  int baud_rate = std::stoi(argv[2]);

  #ifdef DEBUG // useful for debugging
    std::ofstream out("/tmp/pjon_serial.txt");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cerr.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
  #endif

  // std::cerr << "Opening serial..." << std::endl;
  // serial_t serial;
  /* Open /dev/ttyUSB0 with baudrate 115200, and defaults of 8N1, no flow control */
  // if (serial_open(&serial, device, baud_rate) < 0) {
    // fprintf(stderr, "serial_open(): %s\n", serial_errmsg(&serial));
    // exit(1);
  // }
  std::cerr << "Setting serial..." << std::endl;

  PJON<ThroughSerial> bus(BUS_ADDR);

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
        // std::cerr << "port rx'ed port_command length: " << cmd_sz << std::endl;
        // std::cerr << "port rx'ed port_command str: " << port_rx_buffer << std::endl;
        // std::cerr << "*" ;
        port_rx_len = cmd_sz;
        if (cmd_sz == 0) {
          usleep(10000);
        }
      }
      usleep(100);
    }
  }).detach();

  do {
    bus.update();
    bus.receive(100);

    // std::cerr << "." ;

    if (port_rx_len.load() > 0) {
      // std::cerr << "sending packet: " << port_rx_len.load() << std::endl;
      int resp = bus.send_packet(TX_PACKET_ADDR, port_rx_buffer, port_rx_len.load());
      port_rx_len = 0;
    }
  // } while (!std::cout.eof());
  } while (true);

  std::cerr << "exiting..." << std::endl;
  exit(0);
};

