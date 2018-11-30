
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

enum ErlCommandType {
  Invalid = 0,
  ElrCommand = 1,
  ErlData = 2,
  Ping = 3,
  Info = 4,
  Error = 5,
  Settings = 6,
};
typedef enum ErlCommandType erl_cmd_e;

MSGPACK_ADD_ENUM(ErlCommandType);

struct Packet_Info_Wrapper : PJON_Packet_Info {
  Packet_Info_Wrapper(const PJON_Packet_Info &pi) : PJON_Packet_Info(pi)
  {}
};

typedef struct {
  std::uint32_t command;
  std::string binary;
} erlcmd_t;

typedef struct {
  std::string key;
  std::string value;
} erlcmd_meta_str_t;

typedef struct {
  std::string key;
  std::int32_t value;
} erlcmd_meta_int_t;

size_t read_port_cmd(char *buffer, pk_len_t len);
size_t write_port_cmd(uint8_t *buffer, pk_len_t packet_len);

