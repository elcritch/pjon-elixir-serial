ifeq ($(origin PACKET_SIZE), undefined)
$(error Bad Packet Size: $(PACKET_SIZE). Set $PACKET_SIZE with an appropriate value)
endif

# -D=$() \

all:
	$(CXX) -DLINUX \
		-DSERIAL_FREAD_LOOP_DELAY=$(SERIAL_FREAD_LOOP_DELAY) \
		-DSERIAL_SREAD_LOOP_DELAY=$(SERIAL_SREAD_LOOP_DELAY) \
		-DPJON_STRATEGY=$(PJON_STRATEGY) \
		-DPJON_PACKET_MAX_LENGTH=$(PACKET_SIZE) \
		-DPJON_INCLUDE_PACKET_ID=$(PJON_INCLUDE_PACKET_ID) \
		-DPJON_MAX_RECENT_PACKET_IDS=$(PJON_MAX_RECENT_PACKET_IDS) \
		-DTS_RESPONSE_TIME_OUT=$(TS_RESPONSE_TIME_OUT) \
		-DBUS_ADDR=$(BUS_ADDR) \
		-DTX_PACKET_ADDR=$(TX_PACKET_ADDR) \
		-DPJON_SEND_BLOCKING=$(PJON_SEND_BLOCKING) \
		-DPJON_RX_WAIT_TIME=$(PJON_RX_WAIT_TIME) \
		-DTS_MAX_ATTEMPTS=$(TS_MAX_ATTEMPTS) \
		-DDEBUG_MODE=$(DEBUG_MODE) \
		-DDEBUG_LOGFILE="\"$(DEBUG_LOGFILE)\"" \
		-I. -Isrc/PJON/src -Isrc/periphery -Isrc/goodform/include/ -std=gnu++11 \
		src/pjon_serial.cpp -o priv/pjon_serial -lpthread -lm 

clean:
	rm priv/pjon_serial src/*.o

