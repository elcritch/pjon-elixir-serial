
#pragma once

/* Serial ----------------------------------------------------------------- */

#include "serial/serial.h"

#ifndef PJON_SERIAL_TYPE
#define PJON_SERIAL_TYPE serial::Serial *
#endif

int serial_available;

#ifndef PJON_SERIAL_AVAILABLE
#define PJON_SERIAL_AVAILABLE(S) S->available()
#endif

#ifndef PJON_SERIAL_WRITE
#define PJON_SERIAL_WRITE(S, C) S->write(C, 1)
#endif

#ifndef PJON_SERIAL_READ
#define PJON_SERIAL_READ(S) S->read(1).c_str[0]
#endif

#ifndef PJON_SERIAL_FLUSH
#define PJON_SERIAL_FLUSH(S) serialFlush(S)
#endif

/* Timing offset in microseconds between expected and real serial
   byte transmission: */

#ifndef TS_FLUSH_OFFSET
#define TS_FLUSH_OFFSET        152
#endif
