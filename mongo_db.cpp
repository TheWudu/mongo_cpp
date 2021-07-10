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
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/logic_error.hpp>

using namespace mongocxx;

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

void MongoDB::insert(Models::Session rs) {
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

  auto coll = collection("sessions");
  bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(doc.view());
}


bool MongoDB::find(std::string id, Models::Weight* weight) {
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

bool MongoDB::find(std::string id, Models::Session* rs) {
  bsoncxx::document::value query = document{} 
    << "id"   << id
    << bsoncxx::builder::stream::finalize;

  auto coll = collection("sessions");
  bsoncxx::stdx::optional<bsoncxx::document::value> result = coll.find_one(query.view());
 
  if(!result) { 
    return false;
  }
  else {
    build_session(result->view(), rs);
  
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

void MongoDB::build_session(bsoncxx::v_noabi::document::view data, Models::Session* session) {
  session->id = data["id"].get_utf8().value.to_string();
  int64_t ms = (data["start_time"].get_date().value).count();
  session->start_time =  ms / 1000;
  ms = (data["end_time"].get_date().value).count();
  session->end_time =  ms / 1000;
  session->distance = data["distance"].get_int32().value;
  session->duration = data["duration"].get_int32().value;
  if(data["notes"]) {
    session->notes    = data["notes"].get_utf8().value.to_string();
  }
  session->sport_type_id = data["sport_type_id"].get_int32().value;
}

void MongoDB::list_sessions(time_t from, time_t to) {
  bsoncxx::document::value query = document{} 
    << "start_time"   << open_document 
      << "$gte" << time_t_to_b_date(from)
      << "$lte" << time_t_to_b_date(to)
      << close_document
    << bsoncxx::builder::stream::finalize;
    
  std::cout << bsoncxx::to_json(query.view()) << "\n";

  auto coll = collection("sessions");
  mongocxx::cursor cursor = coll.find(query.view());

  std::vector<Models::Session> sessions;
  
  for(auto doc : cursor) {
    std::cout << bsoncxx::to_json(doc) << "\n";
    
    Models::Session rs;
    build_session(doc, &rs);
    sessions.push_back(rs);
  }

  for(auto session: sessions) {
    session.print();
  }
}

void MongoDB::aggregate_stats() {
  using namespace bsoncxx::builder::basic;

  mongocxx::pipeline p{};

/* 
  db.sessions.aggregate([ 
    { $match: { "sport_type_id": { $in: [1,3,4,19] }, "start_time": { $gt: ISODate("2021-04-01"), $lt: ISODate("2022-01-01") } } }, 
    { $group: { _id: "$sport_type_id", overall_distance: { $sum: "$distance" }, overall_duration: { $sum: "$duration" }, overall_count: { $sum: 1 } } }, 
    { $project: { overall_distance: "$overall_distance", overall_duration: "$overall_duration", overall_count: "$overall_count", avg_distance: { $divide: [ "$overall_distance", "$overall_count" ] }, average_pace: { $divide: [ "$overall_duration", "$overall_distance"] } } } 
  ]
*/

  p.match(make_document(kvp("sport_type_id", make_document(kvp("$in", make_array(1,19,7))))));
  p.group(make_document(kvp("_id", "$sport_type_id"), 
                        kvp("overall_distance", make_document(kvp("$sum", "$distance"))),
                        kvp("overall_duration", make_document(kvp("$sum", "$duration"))),
                        kvp("overall_count", make_document(kvp("$sum", 1)))
          ));
  p.project(make_document(kvp("overall_distance", "$overall_distance"),
                          kvp("overall_duration", "$overall_duration"),
                          kvp("overall_count", "$overall_count"),
                          kvp("average_distance", make_document(kvp("$divide", make_array("$overall_distance","$overall_count")))),
                          kvp("average_pace", make_document(kvp("$divide", make_array("$overall_duration","$overall_distance"))))
  ));

  auto cursor = collection("sessions").aggregate(p, mongocxx::options::aggregate{});
  for(auto doc : cursor) {
    std::cout << bsoncxx::to_json(doc) << "\n";
  }
}
