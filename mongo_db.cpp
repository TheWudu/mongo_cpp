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

void MongoDB::insert(Models::Weight weight) {
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
  
bsoncxx::types::b_date time_t_to_b_date(time_t time) {
  std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(time);
  bsoncxx::types::b_date timedate = bsoncxx::types::b_date { tp };
  return timedate;
}

void MongoDB::insert(Models::RunSession rs) {
  auto builder = bsoncxx::builder::stream::document{};

  auto doc_value = builder
    << "id"   << rs.id
    << "start_time" << time_t_to_b_date(rs.start_time)
    << "end_time" << time_t_to_b_date(rs.end_time)
    << "start_time_timezone_offset" << rs.start_time_timezone_offset
    << "distance" << rs.distance
    << "duration" << rs.duration
    << "sport_type_id" << rs.sport_type_id;
  
  if(rs.notes.size() > 0) {
    doc_value << "notes" << rs.notes;
  }
  auto doc = doc_value << bsoncxx::builder::stream::finalize;

  auto coll = collection("run_sessions");
  bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(doc.view());
}


bool MongoDB::find_weight(std::string id, Models::Weight* weight) {
  bsoncxx::document::value query = document{} 
    << "id"   << id
    << bsoncxx::builder::stream::finalize;

  auto coll = collection("weights");
  bsoncxx::stdx::optional<bsoncxx::document::value> result = coll.find_one(query.view());
 
  if(!result) { 
    return false;
  }
  else {
    auto data = result->view();
    std::string i = data["id"].get_utf8().value.to_string();
    float w = data["weight"].get_double().value;
    int64_t ms = (data["date"].get_date().value).count();
    time_t t =  ms / 1000;
  
    *weight = Models::Weight(i, t, w);
    return true;
  }
}

bool MongoDB::exists(std::string colname, std::string id) {
  bsoncxx::document::value query = document{} 
    << "id"   << id
    << bsoncxx::builder::stream::finalize;

  auto coll = collection(colname);
  int64_t count = coll.count_documents(query.view());
 
  if(count == 1) { 
    return true;
  }
  else {
      return false;
  }
}
