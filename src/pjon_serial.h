
#pragma once

/* Serial ----------------------------------------------------------------- */

#ifndef PJON_SERIAL_TYPE
#define PJON_SERIAL_TYPE int16_t
#endif

int serial_available;

#ifndef PJON_SERIAL_AVAILABLE
#define PJON_SERIAL_AVAILABLE(S) ((CSERIAL_OK == c_serial_get_available(S, serial_available)) && serial_available > 0)
#endif

#ifndef PJON_SERIAL_WRITE
#define PJON_SERIAL_WRITE(S, C) serialPutchar(S, C)
#endif

#ifndef PJON_SERIAL_READ
#define PJON_SERIAL_READ(S) serialGetchar(S)
#endif

#ifndef PJON_SERIAL_FLUSH
#define PJON_SERIAL_FLUSH(S) serialFlush(S)
#endif

/* Timing offset in microseconds between expected and real serial
   byte transmission: */

#ifndef TS_FLUSH_OFFSET
#define TS_FLUSH_OFFSET        152
#endif
