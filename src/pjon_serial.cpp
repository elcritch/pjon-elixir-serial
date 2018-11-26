
// For printf used below
#include <stdio.h>
// PJON library
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>

#include <thread>
#include <atomic>
#include <iostream>
#include <string>

#include <thread>
#include <atomic>
#include <iostream>
#include <string>
#include <regex>

/**
    + * Returns the current time in microseconds.
    + */
long getMicros(){
       struct timeval currentTime;
       gettimeofday(&currentTime, NULL);
       return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

long getMillis(){
       struct timeval currentTime;
       gettimeofday(&currentTime, NULL);
       return (currentTime.tv_sec * (int)1e6 + currentTime.tv_usec)/1000;
}

long last_time = getMillis();

#define BUS_ADDR 82
#define TX_PACKET_ADDR 87
#define TS_RESPONSE_TIME_OUT 10000

/* Maximum accepted timeframe between transmission and synchronous
   acknowledgement. This timeframe is affected by latency and CRC computation.
   Could be necessary to higher this value if devices are separated by long
   physical distance and or if transmitting long packets. */

// Max number of old packet ids stored to avoid duplication
#define PJON_INCLUDE_PACKET_ID true
#define PJON_MAX_RECENT_PACKET_IDS 10  // by default 10
#define PJON_PACKET_MAX_LENGTH 256

#define PJON_INCLUDE_TS true // Include only ThroughSerial
// #define PJON_INCLUDE_TSA true // Include only ThroughSerial
#include <PJON.h>

std::atomic<bool> flag_input(false);
std::atomic<bool> flag_prompt(false);

std::regex prompt("\n{0,1}(.{3,5}) \\(([0-9A-Fa-f\\-x]+, )*\\) >"); // Prompt

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */

  char message[PJON_PACKET_MAX_LENGTH];
  memcpy(message, payload, length);

  std::string payload_str(message, length);

  // usleep(1000000);

  std::smatch pmatch;
  bool isprompt = regex_search(payload_str, prompt);
  flag_prompt = { flag_prompt.load() || isprompt };
  // if (isprompt)
    // std::cout << "<prompt-acked/>" << std::endl ;
  // else
    // std::cout << "<no-prompt-ack> " << std::endl ;

  long curr_time = getMillis();
  // printf("<packet(%d)> ", curr_time - last_time);
  printf("<packet(%d)> ", length);
  // printf("<packet> ");
  for (int i = 0; i < length; i++) {
    if (payload[i] == '\0')
      puts("\\0");
    // else if (payload[i] == '\n')
      // puts("\n\\n");
    else
      putchar(payload[i]);
  }

  // printf("\n");
  last_time = curr_time;
  // flush();
}

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    printf("Packet Failure: CONN LOST \n");
  }
  else {
    printf("Packet Failure \n");
  }
}

const int LINE_MAX_LEN = 1024;
int line_len = 0;
char line[LINE_MAX_LEN] = {0};

int main() {
  printf("PJON instantiation... \n");
  PJON<ThroughSerial> bus(BUS_ADDR);
  // PJON<ThroughSerialAsync> bus(BUS_ADDR);
  uint32_t baud_rate = 115200;
  printf("Opening serial... \n");
  int s = serialOpen("/dev/ttyUSB0", baud_rate);
  if(int(s) < 0) printf("Serial open fail!");
  printf("Setting serial... \n");
  bus.strategy.set_serial(s);
  bus.strategy.set_baud_rate(baud_rate);
  bus.set_receiver(receiver_function);
  bus.set_error(error_handler);

  // bus.set_synchronous_acknowledge(true);
  // bus.set_asynchronous_acknowledge(false);

  printf("Opening bus... \n");
  bus.begin();
  printf("Success, initiating BlinkTest repeated transmission... \n");
  // bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device 44 every second

  // while(true) {
  //   bus.update();
  //   bus.receive(1000);
  // }

  std::smatch pmatch;

  std::string a = "Hello World";
  std::cout << "line: `" << a << "` " << (regex_search(a, prompt) == true) << std::endl;

  a = "OK. () > ";
  std::cout << "line: `" << a << "` " << (regex_search(a, prompt) == true) << std::endl;

  a = "\n\rOK. (0, -1, 0, ) > ";
  std::cout << "line: `" << a << "` " << (regex_search(a, prompt) == true) << std::endl;

  a = "W?? (0, -1, 0, 4, 5, ) > ";
  std::cout << "line: `" << a << "` " << (regex_search(a, prompt) == true) << std::endl;

  a = "ERR! (0x12, 0xFFF, 0x4, 0x1, ) > ";
  std::cout << "line: `" << a << "` " << (regex_search(a, prompt) == true) << std::endl;

  a = "Example prompt: 'OK. (0, -1, 0, 4, 5, ) > ' " ;
  std::cout << "line: `" << a << "` " << (regex_search(a, prompt) == true) << std::endl;

  // Send initial Hello
  bus.send_packet(TX_PACKET_ADDR, "\n", 1);

  std::thread([&]{
    while (true) {
      if (flag_input.load() == false && flag_prompt.load() == true) {
        memset(line, 0, LINE_MAX_LEN);

        char *instr = readline(NULL);
        // printf("[read: `%s`]\n", instr);

        int len = strnlen(instr, LINE_MAX_LEN-1);
        memcpy(line, instr, len);
        line[len] = '\n';
        // line[len+1] = '\r';
        // line[len+2] = '\0';
        line_len = len + 1;

        flag_input = {true};
        flag_prompt = {false};
        usleep(10000);
      }
    }
  }).detach();

  // bus.send_repeatedly(TX_PACKET_ADDR, "\n", 1, 4000000); // Send B to device 44 every second

  while(true) {
    bus.update();
    bus.receive(TS_TIME_IN + 5000);
    // delayMicroseconds(1 * 1000);

    if (flag_input.load() == true) {
      // int resp = bus.send_packet(TX_PACKET_ADDR, "\n", 1);
      int resp = bus.send_packet(TX_PACKET_ADDR, line, line_len);
      // printf("[packet result: %d]\n", resp);
      flag_input = {false};

    }

  }
};

