
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

#define BUFFER_SIZE 4096
#define BUS_ADDR 42
#define TX_PACKET_ADDR 47

/* Maximum timeframe between transmission and synchronous acknowledgement. */
#define TS_RESPONSE_TIME_OUT 100000

// Max number of old packet ids stored to avoid duplication
#define PJON_INCLUDE_PACKET_ID true
#define PJON_MAX_RECENT_PACKET_IDS 10
#define PJON_PACKET_MAX_LENGTH 256

#define PJON_INCLUDE_TS true // Include only ThroughSerial


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

