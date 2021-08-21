#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>


class MongoConnection {

private:
  mongocxx::instance mongodb_instance{}; // This should be done only once.
  mongocxx::client client;
  mongocxx::database db; 

  MongoConnection() {
    mongocxx::uri uri("mongodb://localhost:27017");
    client = mongocxx::client(uri);
    db = client["test"];
  };
  
  static MongoConnection* _inst;
  
public:
  static MongoConnection* connection() {
    if (!_inst)
      _inst = new MongoConnection ();
    return _inst;
  }

  ~MongoConnection() {
    if( NULL != MongoConnection::_inst ) {
      delete MongoConnection::_inst;
    }
  }

  mongocxx::collection collection(std::string name);

};
