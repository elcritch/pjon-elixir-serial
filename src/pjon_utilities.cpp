#include "pjon_serial.h"

#include <stdio.h>
#include <stdint.h>
#include <iostream>

#include <unistd.h>

// just cause -- might run this on embedded devices...
#define IS_LITTLE_ENDIAN (!(*(uint16_t *)"\0\xff" < 0x100))

int pjon_serial_timeout_ms = 1000;

int pjon_serial_available(serial_t *serial) {

  // std::cerr << "ser avail... " << std::endl;

  unsigned int count = 0;
  if (serial_input_waiting(serial, &count)) {
    fprintf(stderr, "serial_read(): %s\n", serial_errmsg(serial));
    exit(1);
    // return 0;
  }

  if (count == 0) {
    // std::cerr << "ser avail: " << count << std::endl;
    // usleep(10 * 1000);
  }

  return count;
}

int16_t pjon_serial_read_byte(serial_t *serial) {
  uint8_t data[1] = {0};

  std::cerr << "ser read byte... " ;

  /* Read up to buf size or 2000ms timeout */
  if (serial_read(serial, data, 1, pjon_serial_timeout_ms) < 0) {
    fprintf(stderr, "serial_read(): %s\n", serial_errmsg(serial));
    exit(1);
    // return -1;
  } else {
    std::cerr << " byte: " << data[0] << std::endl;
    return data[0];
  }
}

void pjon_serial_write_byte(serial_t *serial, uint8_t byte) {
  uint8_t data[1] = {byte};

  std::cerr << "ser write byte... " << std::endl;

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

  // pjon_serial_delay((1000000 / (115200 / 8)));
  // pjon_serial_delay(1000);

  if (serial_flush(serial)) {
    fprintf(stderr, "serial_flush(): %s\n", serial_errmsg(serial));
    // exit(1);
  }
}

