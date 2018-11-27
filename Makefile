all:
	g++ -DDEBUG -DLINUX -I. -Isrc/PJON/src -Isrc/periphery -Isrc/goodform/include/ -std=gnu++11 src/pjon_serial.cpp src/pjon_msgpack.cpp -o priv/pjon_serial -lpthread -lm 

clean:
	rm priv/pjon_serial src/*.o

