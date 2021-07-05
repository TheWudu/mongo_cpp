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

#include "mongo_db.hpp"

MongoDB* MongoDB::_inst = 0;

mongocxx::collection MongoDB::collection(std::string name) {
  return db[name];
}

void MongoDB::print_collection(std::string name) {
  mongocxx::cursor cursor = collection(name).find({});
  for(auto doc : cursor) {
    std::cout << bsoncxx::to_json(doc) << "\n";
  }
}

void MongoDB::insert_weight(std::string id, float weight, bsoncxx::types::b_date date) {
  auto builder = bsoncxx::builder::stream::document{};

  bsoncxx::document::value doc_value = builder
    << "id"   << id
    << "type" << "weight"
    << "date" << date
    << "weight" << weight
    << bsoncxx::builder::stream::finalize;

  auto coll = collection("weights");
  bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(doc_value.view());
} 

void MongoDB::insert_weight(Models::Weight weight) {
  auto builder = bsoncxx::builder::stream::document{};

  std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(weight.date);
  bsoncxx::types::b_date date = bsoncxx::types::b_date { tp };
  
  bsoncxx::document::value doc_value = builder
    << "id"   << weight.id
    << "type" << "weight"
    << "date" << date
    << "weight" << weight.weight
    << bsoncxx::builder::stream::finalize;

  auto coll = collection("weights");
  bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(doc_value.view());
}

Models::Weight MongoDB::find_weight(std::string id) {
  bsoncxx::document::value query = document{} 
    << "id"   << id
    << bsoncxx::builder::stream::finalize;

  auto coll = collection("weights");
  bsoncxx::stdx::optional<bsoncxx::document::value> result = coll.find_one(query.view());
  auto data = result->view();
  std::string i = data["id"].get_utf8().value.to_string();
  //float w = data["weight"];
  //time_t t = data["date"];
  
  std::cout << i << std::endl;
  // Models::Weight weight = Models::Weight(i, t, w);

  //weight.print();
  Models::Weight weight; 
  return weight;
}

bool MongoDB::weight_exists(std::string id) {
  auto result = find_weight(id);
  if(result.id.size() > 0 ) {
    // std::cout << bsoncxx::to_json(*result) << "\n";
    return true;
  }
  else {
    return false;
  }
}


