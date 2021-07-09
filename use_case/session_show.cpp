#include "session_show.hpp"

#include "../models/session.hpp"
#include "../mongo_db.hpp"

void UseCase::SessionShow::find(std::string id) {
  Models::Session rs;

  MongoDB* mc = MongoDB::connection();
  if(mc->find(id, &rs)) {
    rs.print();
  }
}
