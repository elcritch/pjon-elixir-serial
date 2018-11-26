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

#define PACKET_SZ 2
typedef size_t pk_len_t;

#define BUFFER_SIZE 4096
#define BUS_ADDR 42
#define TX_PACKET_ADDR 47
#define PJON_SERIAL_TYPE int16_t

/* Maximum timeframe between transmission and synchronous acknowledgement. */
#define TS_RESPONSE_TIME_OUT 10000

// Max number of old packet ids stored to avoid duplication
#define LINUX 1

#define PJON_INCLUDE_PACKET_ID true
#define PJON_MAX_RECENT_PACKET_IDS 10
#define PJON_PACKET_MAX_LENGTH 256

#define PJON_INCLUDE_TS true // Include only ThroughSerial

#include <PJON.h>

/// Read command packet from STDIN
size_t read_port_cmd(char *buffer, pk_len_t len)
{
  pk_len_t packet_len = 0;
  size_t lens_read = fread(&packet_len, PACKET_SZ, 1, stdin);

  #ifdef IS_LITTLE_ENDIAN
    packet_len = swap_endian<pk_len_t>(packet_len);
  #endif

  // exit if we can't read sizeof(pk_len_t) byte's
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
size_t write_port_cmd(byte *buffer, pk_len_t packet_len)
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

  size_t bytes_wrote = fwrite(buffer, sizeof(byte), packet_len, stdout);

  if (bytes_wrote != packet_len) {
    std::cerr
      << "Wrote (less) packet bytes than expected "
      << bytes_wrote * sizeof(byte)
      << " of "
      << packet_len
      << std::endl;

     exit(15);
  }

  // fflush(stdout);
  return bytes_wrote;
};

std::atomic<size_t> port_rx_len;
uint8_t port_rx_buffer[BUFFER_SIZE];

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

int main() {
  PJON<ThroughSerial> bus(BUS_ADDR);
  std::cerr << "Opening serial..." << std::endl;

  Port p;
  p.open("/dev/ttyUSB0");
  p.set(115200, Parity::none, StopBits::one, DataBits::eight);

  int sfd = p.native_handle();

  if (sfd < 0) {
    std::cerr << "Serial open fail!" << std::endl;
    exit(6);
  }

  std::cerr << "Setting serial..." << std::endl;

  bus.strategy.set_serial(s);
  bus.strategy.set_baud_rate(baud_rate);
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
          read_port_cmd(port_rx_buffer, PJON_PACKET_MAX_LENGTH);
        port_rx_len = cmd_sz;
      }
      usleep(20000);
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

