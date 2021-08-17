#include "session_show.hpp"

#include "../models/session.hpp"
#include "../repository/sessions.hpp"

void UseCase::SessionShow::find(std::string id) {
  Models::Session rs;

  Sessions mc;
  if(mc.find(id, &rs)) {
    rs.print();
  }
}
