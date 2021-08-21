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

#include "../helper/time_converter.hpp"
#include "../helper/sport_types.hpp"
#include "../helper/output.hpp"

using namespace mongocxx;

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

#include "sessions.hpp"

mongocxx::collection MongoDB::Sessions::collection() {
  return Base::collection("sessions");
}

void MongoDB::Sessions::insert(Models::Session rs) {
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
    << "pause" << rs.pause
    << "elevation_gain" << rs.elevation_gain
    << "elevation_loss" << rs.elevation_loss
    << "sport_type_id" << rs.sport_type_id;
  
  if(rs.notes.size() > 0) {
    doc_value << "notes" << rs.notes;
  }
  auto doc = doc_value << bsoncxx::builder::stream::finalize;

  bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection().insert_one(doc.view());
}

bool MongoDB::Sessions::find(std::string id, Models::Session* rs) {
  bsoncxx::document::value query = document{} 
    << "id"   << id
    << bsoncxx::builder::stream::finalize;

  bsoncxx::stdx::optional<bsoncxx::document::value> result = collection().find_one(query.view());
 
  if(!result) { 
    return false;
  }
  else {
    build_session(result->view(), rs);
  
    return true;
  }
}


bool MongoDB::Sessions::exists(time_t start_time, int sport_type_id) {
  bsoncxx::document::value query = document{} 
    << "start_time"   << open_document 
      << "$gte" << time_t_to_b_date(start_time - 60)
      << "$lte" << time_t_to_b_date(start_time + 60)
      << close_document
    << "sport_type_id" << sport_type_id 
    << bsoncxx::builder::stream::finalize;
  
  int64_t count = collection().count_documents(query.view());
 
  if(count == 1) { 
    return true;
  }
  return false;
}

bool MongoDB::Sessions::exists(std::string id) {
  bsoncxx::document::value query = document{} 
    << "id"   << id
    << bsoncxx::builder::stream::finalize;

  int64_t count = collection().count_documents(query.view());
 
  if(count == 1) { 
    return true;
  }
  return false;
}

void MongoDB::Sessions::build_session(bsoncxx::v_noabi::document::view data, Models::Session* session) {
  session->id = data["id"].get_utf8().value.to_string();
  int64_t ms  = (data["start_time"].get_date().value).count();
  session->start_time =  ms / 1000;
  ms = (data["end_time"].get_date().value).count();
  session->end_time       =  ms / 1000;
  session->start_time_timezone_offset = data["start_time_timezone_offset"].get_int32().value;
  session->distance       = data["distance"].get_int32().value;
  session->duration       = data["duration"].get_int32().value;
  if(data["pause"]) {
    session->pause          = data["pause"].get_int32().value;
  }
  session->elevation_gain = data["elevation_gain"].get_int32().value;
  session->elevation_loss = data["elevation_loss"].get_int32().value;
  if(data["notes"]) {
    session->notes        = data["notes"].get_utf8().value.to_string();
  }
  session->sport_type_id  = data["sport_type_id"].get_int32().value;
}

void MongoDB::Sessions::list(time_t from, time_t to, std::vector<int> sport_type_ids, std::string notes) {
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

  mongocxx::cursor cursor = collection().find(query.view(), opts);

  std::vector<Models::Session> sessions;
  
  for(auto doc : cursor) {
    Models::Session rs;
    build_session(doc, &rs);
    sessions.push_back(rs);
  }

  Output::print_session_list(sessions);
}

bool MongoDB::Sessions::delete_one(std::string id) {
  bsoncxx::document::value query = document{} 
    << "id"   << id
    << bsoncxx::builder::stream::finalize;

  auto result = collection().delete_one(query.view());
 
  if(!result) { 
    return false;
  }
  else {
    return (result->deleted_count() == 1);
  }
}

uint32_t MongoDB::Sessions::delete_many(time_t const from, time_t const to) {
  bsoncxx::document::value query = document{} 
    << "start_time" << open_document 
      << "$gte" << time_t_to_b_date(from)
      << "$lte" << time_t_to_b_date(to)
      << close_document
    << bsoncxx::builder::stream::finalize;

  auto result = collection().delete_many(query.view());
 
  if(!result) { 
    return 0;
  }
  else {
    return result->deleted_count();
  }
}
