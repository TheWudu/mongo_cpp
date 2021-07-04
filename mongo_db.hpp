#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include "models/weight.hpp"    

class MongoDB {

private:
  mongocxx::instance mongodb_instance{}; // This should be done only once.
  mongocxx::client client;
  mongocxx::database db; 

  MongoDB() {
    mongocxx::uri uri("mongodb://localhost:27017");
    client = mongocxx::client(uri);
    db = client["test"];
  };

  static MongoDB* _inst;

public:

  static MongoDB* connection() {
    if (!_inst)
          _inst = new MongoDB ();
    return _inst;
  }

  ~MongoDB() {
    if( NULL != MongoDB::_inst ) {
      delete MongoDB::_inst;
    }
  }
  
  mongocxx::collection collection(std::string name);

  void insert_weight(std::string id, float weight, bsoncxx::types::b_date date);
  void insert_weight(Weight weight);
  bsoncxx::stdx::optional<bsoncxx::document::value> find_weight(std::string id);
  bool weight_exists(std::string id);

  void print_collection(std::string name);
};

