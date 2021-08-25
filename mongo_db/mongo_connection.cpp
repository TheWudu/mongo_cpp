#include "mongo_connection.hpp"

MongoConnection* MongoConnection::_inst = 0;

mongocxx::collection MongoConnection::collection(std::string name) {
  auto c = client();
  return (*c)["test"][name];
}

mongocxx::pool::entry MongoConnection::client() {
  return _pool->acquire();
}
