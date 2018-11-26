all:
	g++ -DLINUX -I. -Isrc/PJON/src -Isrc/serial-1.2.1/include -std=gnu++11 src/pjon_serial.cpp src/serial-1.2.1/src/serial.cc -o priv/pjon_serial -lpthread -lcrypt -lm -lrt

clean:
	rm priv/pjon_serial src/*.o

