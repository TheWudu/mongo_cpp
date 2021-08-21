#include "mongo_connection.hpp"


MongoConnection* MongoConnection::_inst = 0;

mongocxx::collection MongoConnection::collection(std::string name) {
  return db[name];
}
