
#include "pjon_serial.h"

#include "goodform/variant.hpp"
#include "goodform/form.hpp"
#include "goodform/msgpack.hpp"


#include "goodform/src/variant.cpp"
#include "goodform/src/form.cpp"
#include "goodform/src/msgpack.cpp"

std::string pack_command(erl_cmd_e type, char *data) {
  std::stringstream ss;
  goodform::any var;

  var = goodform::object_t {
    {"command", type},
    {"binary", std::string(data)}
  };

  goodform::msgpack::serialize(var, ss);

  return ss.str();
}

erlcmd_t unpack_command(char *rawcommand) {
  std::stringstream ss; 
  goodform::any var;

  ss << rawcommand;
  goodform::msgpack::deserialize(ss, var);

  goodform::form form(var);

  erlcmd_t cmdmsg;
    
  cmdmsg.command = form.at("command").uint32().val();
  cmdmsg.binary = form.at("binary").string().val();

  if (form.is_good())
    {
      return cmdmsg;
    } else {
    return { Invalid, std::string("")};
  }
}
