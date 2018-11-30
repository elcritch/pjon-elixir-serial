
#pragma once

#include <stdio.h>
#include <stdint.h>
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

#define MSGPACK_USE_DEFINE_MAP

#include <msgpack.hpp>

#define PJON_INCLUDE_TS true // Include only ThroughSerial

#include <PJON.h>

#define PACKET_SZ 2
typedef uint16_t pk_len_t;

enum ErlCommsType {
  Invalid = 0,
  SendPjonPacket = 1,
  ReceivedPjonPacket = 2,
  PjonPacketError = 3,
  Ping = 4,
  Info = 5,
  Settings = 6,
};

MSGPACK_ADD_ENUM(ErlCommsType);

typedef std::vector<unsigned char> message_t;
typedef std::vector<char> message_str_t;

struct ErlCommsPacketInfo : PJON_Packet_Info {
  ErlCommsPacketInfo(const PJON_Packet_Info &pi) : PJON_Packet_Info(pi)
  {}
  MSGPACK_DEFINE(header, id, receiver_id, receiver_bus_id, sender_id, sender_bus_id, port);
};

struct ErlCommsPacketTx {
  std::uint32_t addr;
  message_t message;
  MSGPACK_DEFINE(addr, message);
};

struct ErlCommsPacketRx {
  message_t message;
  ErlCommsPacketInfo packet_info;
  ErlCommsPacketRx(message_t msg, ErlCommsPacketInfo pi) : message(msg), packet_info(pi) {}
  MSGPACK_DEFINE(message, packet_info);
};

struct ErlCommsPacketError {
  uint8_t error_code;
  message_str_t message;
  ErlCommsPacketError(uint8_t ec, message_str_t msg) : error_code(ec), message(msg)  {}
  MSGPACK_DEFINE(error_code, message);
};

struct ErlCommsInfo {
  message_t message;
  ErlCommsInfo(message_t msg) : message(msg) {}
  MSGPACK_DEFINE(message);
};

template <typename ErlCommsPacket>
std::tuple<ErlCommsType, ErlCommsPacket> erl_comms_packet(ErlCommsType tp, ErlCommsPacket pkt) {
  return std::tuple<ErlCommsType, ErlCommsPacket>(tp, pkt);
}

size_t read_port_cmd(char *buffer, pk_len_t len);
size_t write_port_cmd(uint8_t *buffer, pk_len_t packet_len);

