all:
	g++ -DLINUX -I. -Isrc/PJON/src -std=gnu++11 src/pjon_serial.cpp -o priv/pjon_serial -lpthread -lcrypt -lm -lrt

clean:
	rm priv/pjon_serial src/*.o

