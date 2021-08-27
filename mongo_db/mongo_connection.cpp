#include "mongo_connection.hpp"

MongoConnection* MongoConnection::_inst = 0;

mongocxx::collection MongoConnection::collection(std::string name) {
  auto c = client();
  std::string db_name = Config::instance()->mongo_db_name();
  return (*c)[db_name][name];
}

mongocxx::pool::entry MongoConnection::client() {
  return _pool->acquire();
}
