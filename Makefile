all:
	g++ -DLINUX -I. -Isrc/PJON/src -Isrc/periphery -std=gnu++11 src/pjon_serial.cpp src/pjon_utilities.cpp src/periphery/serial.c -o priv/pjon_serial -lpthread -lm 

clean:
	rm priv/pjon_serial src/*.o

