#include "pjon_serial.h"

#include <stdio.h>
#include <stdint.h>

// just cause -- might run this on embedded devices...
#define IS_LITTLE_ENDIAN (!(*(uint16_t *)"\0\xff" < 0x100))

template <typename T>
T swap_endian(T u)
{

  union
  {
    T u;
    uint8_t u8[sizeof(T)];
  } source, dest;

  source.u = u;

  for (size_t k = 0; k < sizeof(T); k++)
    {
      dest.u8[k] = source.u8[sizeof(T) - k - 1];
    }

  return dest.u;
}

int pjon_serial_timeout_ms = 1;

int pjon_serial_available(serial_t *serial) {
  unsigned int count = 0;
  if (serial_input_waiting(serial, &count)) {
    fprintf(stderr, "serial_read(): %s\n", serial_errmsg(serial));
    // exit(1);
    return 0;
  }

  return count;
}

int16_t pjon_serial_read_byte(serial_t *serial) {
  uint8_t data[1] = {0};

  /* Read up to buf size or 2000ms timeout */
  if (serial_read(serial, data, 1, pjon_serial_timeout_ms) < 0) {
    fprintf(stderr, "serial_read(): %s\n", serial_errmsg(serial));
    // exit(1);
    return -1;
  } else {
    return data[0];
  }
}

void pjon_serial_write_byte(serial_t *serial, uint8_t byte) {
  uint8_t data[1] = {byte};

  /* Write to the serial port */
  if (serial_write(serial, data, 1) < 0) {
    fprintf(stderr, "serial_write(): %s\n", serial_errmsg(serial));
    // exit(1);
  }
}

void pjon_serial_flush(serial_t *serial) {
  // Note: Copying ... don't want to refix this on RPI
  /* On RPI flush fails to wait until all bytes are transmitted
     here RPI forced to wait blocking using delayMicroseconds */

  pjon_serial_delay((1000000 / (115200 / 8)));

  if (serial_flush(serial)) {
    fprintf(stderr, "serial_flush(): %s\n", serial_errmsg(serial));
    // exit(1);
  }
}

