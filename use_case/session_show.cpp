#include "session_show.hpp"

#include "../models/session.hpp"
#include "../mongo_db/sessions.hpp"

void UseCase::SessionShow::find(std::string id) {
  Models::Session rs;

  MongoDB::Sessions sessions;
  if(sessions.find(id, &rs)) {
    rs.print();
  }
}
