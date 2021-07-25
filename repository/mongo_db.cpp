#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility> // for std::pair

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

#include "helper/time_converter.hpp"
#include "helper/sport_types.hpp"
#include "output.hpp"

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
  
bsoncxx::types::b_date MongoDB::time_t_to_b_date(time_t time) {
  std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(time);
  bsoncxx::types::b_date timedate = bsoncxx::types::b_date { tp };
  return timedate;
}

std::string MongoDB::new_object_id() {
  bsoncxx::oid oid{};
  return oid.to_string();
}

void MongoDB::insert(Models::Session rs) {
  auto builder = bsoncxx::builder::stream::document{};

  auto doc_value = builder
    << "id"   << rs.id
    << "year" << gmtime(&rs.start_time)->tm_year + 1900
    << "month" << gmtime(&rs.start_time)->tm_mon + 1
    << "start_time" << time_t_to_b_date(rs.start_time)
    << "end_time" << time_t_to_b_date(rs.end_time)
    << "start_time_timezone_offset" << rs.start_time_timezone_offset
    << "distance" << rs.distance
    << "duration" << rs.duration
    << "elevation_gain" << rs.elevation_gain
    << "elevation_loss" << rs.elevation_loss
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
    float w    = data["weight"].get_double().value;
    int64_t ms = (data["date"].get_date().value).count();
    time_t t   =  ms / 1000;
  
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


bool MongoDB::exists(time_t start_time, int sport_type_id) {
  bsoncxx::document::value query = document{} 
    << "start_time"   << open_document 
      << "$gte" << time_t_to_b_date(start_time - 60)
      << "$lte" << time_t_to_b_date(start_time + 60)
      << close_document
    << "sport_type_id" << sport_type_id 
    << bsoncxx::builder::stream::finalize;
  
  auto coll = collection("sessions");

  int64_t count = coll.count_documents(query.view());
 
  if(count == 1) { 
    return true;
  }
  return false;
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
  return false;
}

void MongoDB::build_session(bsoncxx::v_noabi::document::view data, Models::Session* session) {
  session->id = data["id"].get_utf8().value.to_string();
  int64_t ms  = (data["start_time"].get_date().value).count();
  session->start_time =  ms / 1000;
  ms = (data["end_time"].get_date().value).count();
  session->end_time       =  ms / 1000;
  session->distance       = data["distance"].get_int32().value;
  session->duration       = data["duration"].get_int32().value;
  session->elevation_gain = data["elevation_gain"].get_int32().value;
  session->elevation_loss = data["elevation_loss"].get_int32().value;
  if(data["notes"]) {
    session->notes        = data["notes"].get_utf8().value.to_string();
  }
  session->sport_type_id  = data["sport_type_id"].get_int32().value;
}

void MongoDB::list_sessions(time_t from, time_t to, std::vector<int> sport_type_ids, std::string notes) {
  auto matcher = bsoncxx::builder::stream::document{};
  matcher << "start_time"   << open_document 
      << "$gte" << time_t_to_b_date(from)
      << "$lte" << time_t_to_b_date(to)
      << close_document;
  sport_type_matcher(matcher, sport_type_ids);
  
  if(notes.size() > 0) {
    matcher << "notes" << bsoncxx::types::b_regex{notes}; 
  }
  auto query = matcher << bsoncxx::builder::stream::finalize;

  auto order = bsoncxx::builder::stream::document{} 
    << "start_time" << 1 
    << bsoncxx::builder::stream::finalize;

  auto opts = mongocxx::options::find{};
  opts.sort(order.view());

  auto coll = collection("sessions");
  mongocxx::cursor cursor = coll.find(query.view(), opts);

  std::vector<Models::Session> sessions;
  
  for(auto doc : cursor) {
    Models::Session rs;
    build_session(doc, &rs);
    sessions.push_back(rs);
  }

  Output::print_session_list(sessions);
}

template <class T>
bsoncxx::builder::basic::array MongoDB::vector_to_array(std::vector<T> vec) {
  bsoncxx::builder::basic::array a = bsoncxx::builder::basic::array();
  for(auto v : vec) {
    a.append(v);
  }
  return a;
}

void MongoDB::sport_type_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> sport_type_ids) {
  using namespace bsoncxx::builder::basic;
  if(sport_type_ids.size() > 0) {
    matcher << "sport_type_id" << open_document <<
      "$in" << vector_to_array(sport_type_ids) << close_document;
  }
}
  
void MongoDB::year_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> years) {
  if(years.size() > 0) {
    matcher << "year" << open_document <<
      "$in" << vector_to_array(years) << close_document;
  }
}
 
