#include "pjon_serial.h"

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>


#include <PJON.h>

void pjon_serial_delay(long ms) {
  PJON_DELAY_MICROSECONDS(ms);
}

/// Read command packet from STDIN
size_t read_port_cmd(char *buffer, pk_len_t len)
{
  pk_len_t packet_len = 0;
  size_t lens_read = fread(&packet_len, PACKET_SZ, 1, stdin);

  #ifdef IS_LITTLE_ENDIAN
    packet_len = swap_endian<pk_len_t>(packet_len);
  #endif

  // exit if we can't read sizeof(pk_len_t) uint8_t's
  if (lens_read == 0) {
    exit(2);
  }
  else if (lens_read > 1) {
    std::cerr << "Error reading length of cmd packet " << std::endl;
    exit(3);
  }

  if (packet_len >= len) {
    std::cerr << "Packet larger than buffer " << packet_len << std::endl;
    exit(4);
  }

  // if we can't read complete message data, exit
  size_t bytes_read = fread(buffer, 1, packet_len, stdin);

  std::cerr << "Port read: " << bytes_read << std::endl;

  if (bytes_read != packet_len) {
    std::cerr
      << "Read (less) packet bytes than expected "
      << bytes_read
      << packet_len
      << std::endl;

    exit(5);
  }

  return packet_len;
}

/// Write command packet to STDOUT
size_t write_port_cmd(uint8_t *buffer, pk_len_t packet_len)
{

  pk_len_t len_out = packet_len;

  std::cerr << "Writing bytes: " << len_out << std::endl;


  // swap for endianness
  #ifdef IS_LITTLE_ENDIAN
    len_out = swap_endian<pk_len_t>(len_out);
  #endif

  std::cerr << "Writing: bytes swapped: " << len_out << std::endl;
  size_t lens_wrote = fwrite(&len_out, sizeof(len_out), 1, stdout);

  if (lens_wrote != 1) {
    std::cerr << "Error writing length of data packet " << std::endl;
    exit(13);
  }

  size_t bytes_wrote = fwrite(buffer, sizeof(uint8_t), packet_len, stdout);

  if (bytes_wrote != packet_len) {
    std::cerr
      << "Wrote (less) packet bytes than expected "
      << bytes_wrote * sizeof(uint8_t)
      << " of "
      << packet_len
      << std::endl;

     exit(15);
  }

  // fflush(stdout);
  return bytes_wrote;
};

std::atomic<size_t> port_rx_len;
char port_rx_buffer[BUFFER_SIZE];

void receiver_function(uint8_t *payload,
                       uint16_t length,
                       const PJON_Packet_Info &packet_info)
{
  write_port_cmd(payload, length);
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
  int baud_rate = atoi(argv[2]);

  std::cerr << "Opening serial..." << std::endl;
  serial_t serial;
  /* Open /dev/ttyUSB0 with baudrate 115200, and defaults of 8N1, no flow control */
  if (serial_open(&serial, device, baud_rate) < 0) {
    fprintf(stderr, "serial_open(): %s\n", serial_errmsg(&serial));
    exit(1);
  }
  std::cerr << "Setting serial..." << std::endl;

  PJON<ThroughSerial> bus(BUS_ADDR);
  bus.strategy.set_serial(&serial);
  // bus.strategy.set_baud_rate(baud_rate);
  bus.set_receiver(receiver_function);
  bus.set_error(error_handler);

  std::cerr << "Opening bus" << std::endl;

  bus.begin();
  std::cerr << "Success, starting communication" << std::endl;

  // Thread to handle reading input port commands
  std::thread([&]{
    while (true) {
      if (port_rx_len.load() == 0) {
        pk_len_t cmd_sz =
          read_port_cmd( port_rx_buffer, PJON_PACKET_MAX_LENGTH);
        port_rx_len = cmd_sz;
      }
      PJON_DELAY_MICROSECONDS(20000);
    }
  }).detach();

  do {
    bus.update();
    bus.receive(TS_TIME_IN + 5000);

    if (port_rx_len.load() > 0) {
      int resp = bus.send_packet(TX_PACKET_ADDR, port_rx_buffer, port_rx_len);
      port_rx_len = 0;
    }
  } while (!feof(stdin));

  exit(0);
};

