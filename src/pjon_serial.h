
#pragma once

extern "C" {
  #include "periphery/serial.h"
}

#define PACKET_SZ 2
typedef uint16_t pk_len_t;

#define BUFFER_SIZE 4096
#define BUS_ADDR 42
#define TX_PACKET_ADDR 47

/* Maximum timeframe between transmission and synchronous acknowledgement. */
#define TS_RESPONSE_TIME_OUT 10000

// Max number of old packet ids stored to avoid duplication
#define PJON_INCLUDE_PACKET_ID true
#define PJON_MAX_RECENT_PACKET_IDS 10
#define PJON_PACKET_MAX_LENGTH 256

#define PJON_INCLUDE_TS true // Include only ThroughSerial

size_t read_port_cmd(char *buffer, pk_len_t len);
size_t write_port_cmd(uint8_t *buffer, pk_len_t packet_len);

// Serial Shims
extern int pjon_serial_available(serial_t *serial);
extern int16_t pjon_serial_read_byte(serial_t *serial);
extern void pjon_serial_write_byte(serial_t *serial, uint8_t byte);
extern void pjon_serial_flush(serial_t *serial);
extern void pjon_serial_delay(long ms);


/* Serial ----------------------------------------------------------------- */


#ifndef PJON_SERIAL_TYPE
#define PJON_SERIAL_TYPE serial_t *
#endif

#ifndef PJON_SERIAL_AVAILABLE
#define PJON_SERIAL_AVAILABLE(S) pjon_serial_available(S)
#endif

#ifndef PJON_SERIAL_WRITE
#define PJON_SERIAL_WRITE(S, C) pjon_serial_write_byte(S, C)
#endif

#ifndef PJON_SERIAL_READ
#define PJON_SERIAL_READ(S) pjon_serial_read_byte(S)
#endif

#ifndef PJON_SERIAL_FLUSH
#define PJON_SERIAL_FLUSH(S) pjon_serial_flush(S)
#endif

/* Timing offset in microseconds between expected and real serial
   byte transmission: */

#ifndef TS_FLUSH_OFFSET
#define TS_FLUSH_OFFSET        152
#endif
