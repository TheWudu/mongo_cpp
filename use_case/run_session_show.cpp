#include "run_session_show.hpp"

#include "../models/run_session.hpp"
#include "../mongo_db.hpp"

void UseCase::RunSessionShow::find(std::string id) {
  Models::RunSession rs;

  MongoDB* mc = MongoDB::connection();
  if(mc->find(id, &rs)) {
    rs.print();
  }
}
