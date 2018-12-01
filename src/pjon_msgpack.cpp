
#include "pjon_serial.h"


std::string erl_comms_error_msg(uint8_t code, uint16_t data) {
  std::stringstream err_msg_ss;

  if(code == PJON_CONNECTION_LOST) {
    err_msg_ss << "Connection with device ID "
               << bus.packets[data].content[0]
               << " is lost.";
  }
  if(code == PJON_PACKETS_BUFFER_FULL) {
    err_msg_ss << "Packet buffer is full, has now a length of "
               << data
               << "Possible wrong bus configuration!"
               << "higher PJON_MAX_PACKETS if necessary.";
  }
  if(code == PJON_CONTENT_TOO_LONG) {
    err_msg_ss << "Content is too long, length: "
               << data;
  }

  // Well, if it works -- better way to convert to string<uint8_t> ?
  std::string tmpstr(err_msg_ss.str());
  const char *err_buffer = tmpstr.c_str();
  std::vector<char> message( err_buffer, err_buffer + tmpstr.length());

  ErlCommsPacketError cmd_body(code, message);

  auto comms_pkt = erl_comms_packet(ReceivedPjonPacket, cmd_body);

  // serialize the object into the buffer.
  std::stringstream buffer;
  msgpack::pack(buffer, comms_pkt);

  // deserialize the buffer into msgpack::object instance.
  buffer.seekg(0);
  std::string packed(buffer.str());

  return packed;
}

std::string pack_erl_comms_send_cmd(
    uint8_t *payload,
    uint16_t length,
    const PJON_Packet_Info &packet_info)
{
  std::vector<unsigned char> message(payload, payload + length );

  ErlCommsPacketRx cmd_body(message, packet_info);

  auto comms_pkt = erl_comms_packet(ReceivedPjonPacket, cmd_body);

  // serialize the object into the buffer.
  std::stringstream buffer;
  msgpack::pack(buffer, comms_pkt);

  // deserialize the buffer into msgpack::object instance.
  buffer.seekg(0);
  std::string packed(buffer.str());

  return packed;
}

erl_comms_object parse_erl_comms(char *buffer, pk_len_t cmd_sz) {
  std::string str(buffer, cmd_sz);
  msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());
  msgpack::object mobject = oh.get();

#ifdef DEBUG_MODE
  std::cerr << "Read erl_comms message: " << mobject << std::endl;
#endif // DEBUG_MODE

  erl_comms_object comms_obj;
  mobject.convert(comms_obj);

  return comms_obj;
}

void handle_erl_comms_command() {
  char buffer[BUFFER_MAX];
  pk_len_t cmd_sz = read_port_cmd<pk_len_t>( buffer, BUFFER_MAX);

  if (cmd_sz == 0) {
    std::cerr << "STDIN closed, exiting. " << std::endl;
    exit(0);
  }

  try {
    erl_comms_object comms_obj = parse_erl_comms(buffer, cmd_sz);

    switch (comms_obj.get<0>()) {
    case SendPjonPacket: {
      ErlCommsPacketTx packet_next;
      comms_obj.get<1>().convert(packet_next);
      break;
    }
    case Invalid:
    case ReceivedPjonPacket:
    case PjonPacketError:
    case Info:
    case Settings:
    default:
      std::cerr << "Not Implemented yet";
      exit(2);
    }

  } catch (msgpack::type_error &exc) {
    std::cerr << "Error parsing erl_comms packet...\n";
    std::cerr << exc.what();
    exit(1);
  }
}
