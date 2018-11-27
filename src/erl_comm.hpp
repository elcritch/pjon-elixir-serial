

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <climits>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

#include "pjon_serial.h"
// template <typename PacketLenType> size_t packet_len(PacketLenType len);
 uint8_t parse_packet_len(uint8_t len) { return len; }
 uint16_t parse_packet_len(uint16_t len) { return ntohs(len); }
 uint32_t parse_packet_len(uint32_t len) { return ntohl(len); }

uint8_t pack_packet_len(uint8_t len) { return len; }
uint16_t pack_packet_len(uint16_t len) { return htons(len); }
uint32_t pack_packet_len(uint32_t len) { return htonl(len); }

#include <stdio.h>
#include <sys/select.h>

int is_closed(FILE *fp) {
  // int fd = fileno(fp);
  long result = ftell(fp);
  std::cerr << "stdin ftell: " << result << std::endl;
  std::cerr << "stdin ftell errno: " << errno << std::endl;
  return result < 0 && errno == EBADF;
}

/// Read command packet from STDIN
/// the type `PacketLenType` must be the same as the Erlang Packet size (1, 2, or 4 bytes long, eg. char, uint16_t, or uint32_t respectively)

template <typename PacketLenType>
size_t read_port_cmd(char *buffer, PacketLenType len)
{
  PacketLenType packet_len = 0;

  size_t lens_read = fread(&packet_len,
                           sizeof(PacketLenType),
                           1,
                           stdin);

  packet_len = parse_packet_len(packet_len);

  // if we can't read sizeof(PacketLenType) char's, exit
  if (lens_read == 0) {
    std::cerr << "Read Zero Packet " << std::endl;
    return 0;
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

  // std::cerr << "Port read: " << bytes_read << std::endl;

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
/// the type `PacketLenType` must be the same as the Erlang Packet size (1, 2, or 4 bytes long, eg. char, uint16_t, or uint32_t respectively)
template <typename PacketLenType>
size_t write_port_cmd(char *buffer, PacketLenType packet_len)
{

  PacketLenType len_out = packet_len;

  // std::cerr << "Writing bytes: " << len_out << std::endl;

  // Pack Byte
  len_out = pack_packet_len(len_out);


  // std::cerr << "Writing: bytes swapped: " << len_out << std::endl;

  size_t lens_wrote = fwrite(&len_out,
                                sizeof(len_out),
                                1,
                                stdout);

  if (lens_wrote != 1) {
    std::cerr << "Error writing length of data packet" << std::endl;
    exit(13);
  }

  // std::cerr << " writing port_command of packet: " << packet_len << " " << buffer << std::endl;

  size_t bytes_wrote = fwrite(buffer, sizeof(char), packet_len, stdout);

  if (bytes_wrote != packet_len) {
    std::cerr
      << "Wrote (less) packet bytes than expected "
      << bytes_wrote * sizeof(char)
      << " of "
      << packet_len
      << std::endl;

     exit(15);
  }

  fflush(stdout);
  return bytes_wrote;
};

