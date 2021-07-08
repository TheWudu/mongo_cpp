#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include "models/weight.hpp"    
#include "models/run_session.hpp"

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

  void insert(Models::Weight weight);
  void insert(Models::RunSession rs);

  bool find(std::string id, Models::Weight* weight);
  bool find(std::string id, Models::RunSession* run_session);

  bool exists(std::string colname, std::string id);

  void aggregate_stats();
  void print_collection(std::string name);
};

