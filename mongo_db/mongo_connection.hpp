#pragma once

#include <iostream>
#include <mongocxx/pool.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/stdx/make_unique.hpp>

#include "../config.hpp"

class MongoConnection {

private:
  mongocxx::instance mongodb_instance{}; // This should be done only once.

  MongoConnection() {
    Config* c = Config::instance();
    mongocxx::uri uri { c->mongo_connection() };

    std::cout << "Connecting to " << c->mongo_connection() << std::endl;
    _pool = bsoncxx::stdx::make_unique<mongocxx::pool>(std::move(uri));
  };
  
  static MongoConnection* _inst;
  
public:
  std::unique_ptr<mongocxx::pool> _pool = nullptr;

  static MongoConnection* connection() {
    static MongoConnection _inst;
    return &_inst;
  }

  ~MongoConnection() {
    if( NULL != MongoConnection::_inst ) {
      delete MongoConnection::_inst;
    }
  }

  mongocxx::pool::entry client();

  mongocxx::collection collection(std::string name);

};
