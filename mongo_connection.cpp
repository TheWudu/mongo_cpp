#include <cstdint>
#include <iostream>
#include <vector>

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>


using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

#include "mongo_connection.hpp"

MongoConnection* MongoConnection::_inst = 0;

void MongoConnection::print_collection() {
  mongocxx::cursor cursor = this->collection.find({});
  for(auto doc : cursor) {
    std::cout << bsoncxx::to_json(doc) << "\n";
  }
}

void MongoConnection::insert_weight(std::string id, float weight, char* date) {
  auto builder = bsoncxx::builder::stream::document{};

  bsoncxx::document::value doc_value = builder
    << "id"   << id
    << "type" << "weight"
    << "date" << date
    << "weight" << weight
    << bsoncxx::builder::stream::finalize;

  bsoncxx::stdx::optional<mongocxx::result::insert_one> result = this->collection.insert_one(doc_value.view());
} 

bsoncxx::stdx::optional<bsoncxx::document::value> MongoConnection::find_weight(std::string date) {
  bsoncxx::document::value query = document{} 
    << "date"   << date
    << bsoncxx::builder::stream::finalize;

  bsoncxx::stdx::optional<bsoncxx::document::value> result = collection.find_one(query.view());
  return result;
}

bool MongoConnection::weight_exists(std::string date) {
  auto result = find_weight(date);
  if(result) {
    std::cout << bsoncxx::to_json(*result) << "\n";
    return true;
  }
  else {
    return false;
  }
}


