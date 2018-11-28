ifeq ($(origin PACKET_SIZE), undefined)
$(error Bad Packet Size: $(PACKET_SIZE). Set $PACKET_SIZE with an appropriate value)
endif

all:
	g++ -DLINUX \
		-DPJON_PACKET_MAX_LENGTH=$(PACKET_SIZE) \
		-DPJON_INCLUDE_PACKET_ID=$(PJON_INCLUDE_PACKET_ID) \
		-DPJON_MAX_RECENT_PACKET_IDS=$(PJON_MAX_RECENT_PACKET_IDS) \
		-DTS_RESPONSE_TIME_OUT=$(TS_RESPONSE_TIME_OUT) \
		-DBUS_ADDR=$(BUS_ADDR) \
		-DTX_PACKET_ADDR=$(TX_PACKET_ADDR) \
		-DDEBUG \
		-I. -Isrc/PJON/src -Isrc/periphery -Isrc/goodform/include/ -std=gnu++11 \
		src/pjon_serial.cpp -o priv/pjon_serial -lpthread -lm 

clean:
	rm priv/pjon_serial src/*.o

