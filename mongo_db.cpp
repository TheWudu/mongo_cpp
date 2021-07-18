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
  
bsoncxx::types::b_date time_t_to_b_date(time_t time) {
  std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(time);
  bsoncxx::types::b_date timedate = bsoncxx::types::b_date { tp };
  return timedate;
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
  session->elevation_gain = data["elevation_gain"].get_int32().value;
  session->elevation_loss = data["elevation_loss"].get_int32().value;
  if(data["notes"]) {
    session->notes    = data["notes"].get_utf8().value.to_string();
  }
  session->sport_type_id = data["sport_type_id"].get_int32().value;
}

void MongoDB::list_sessions(time_t from, time_t to, std::vector<int> sport_type_ids, std::string notes) {
   auto builder = bsoncxx::builder::stream::document{};
   auto doc = builder << "start_time"   << open_document 
      << "$gte" << time_t_to_b_date(from)
      << "$lte" << time_t_to_b_date(to)
      << close_document;

  if(sport_type_ids.size() > 0) {
    doc << "sport_type_id" << 
      open_document << "$in" << vector_to_array(sport_type_ids) << close_document;
  }
  if(notes.size() > 0) {
    doc << "notes" << bsoncxx::types::b_regex{notes}; 
  }
  auto query = doc << bsoncxx::builder::stream::finalize;

  // std::cout << bsoncxx::to_json(query.view()) << "\n";

  auto coll = collection("sessions");
  mongocxx::cursor cursor = coll.find(query.view());

  std::vector<Models::Session> sessions;
  
  for(auto doc : cursor) {
    // std::cout << bsoncxx::to_json(doc) << "\n";
    
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

void MongoDB::aggregate_stats(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<std::string> grouping) {

  aggregate_basic_statistics(years, sport_type_ids, grouping);
  aggregate_weekdays(years, sport_type_ids);
  aggregate_hour_of_day(years, sport_type_ids);
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
 
 
void MongoDB::aggregate_basic_statistics(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<std::string> grouping) {
  using namespace bsoncxx::builder::basic;

  mongocxx::pipeline p{};

/* 
  db.sessions.aggregate([ 
    { $match: { "sport_type_id": { $in: [1,3,4,19] }, "start_time": { $gt: ISODate("2021-04-01"), $lt: ISODate("2022-01-01") } } }, 
    { $group: { _id: "$sport_type_id", overall_distance: { $sum: "$distance" }, overall_duration: { $sum: "$duration" }, overall_count: { $sum: 1 } } }, 
    { $project: { overall_distance: "$overall_distance", overall_duration: "$overall_duration", overall_count: "$overall_count", avg_distance: { $divide: [ "$overall_distance", "$overall_count" ] }, average_pace: { $divide: [ "$overall_duration", "$overall_distance"] } } } 
  ] years
*/
  auto matcher = bsoncxx::builder::stream::document {};
  sport_type_matcher(matcher, sport_type_ids);
  year_matcher(matcher, years);

  auto group_by = bsoncxx::builder::stream::document {};
  auto sorter = bsoncxx::builder::stream::document {};
  for(auto g : grouping) {
    group_by << g << "$" + g;
    sorter << "_id." + g << 1;
  }

  p.match(matcher.view());
  p.group(make_document(kvp("_id", group_by.view()), 
                        kvp("overall_distance", make_document(kvp("$sum", "$distance"))),
                        kvp("overall_duration", make_document(kvp("$sum", "$duration"))),
                        kvp("overall_elevation_gain", make_document(kvp("$sum", "$elevation_gain"))),
                        kvp("overall_elevation_loss", make_document(kvp("$sum", "$elevation_loss"))),
                        kvp("overall_count", make_document(kvp("$sum", 1)))
          ));
  p.project(make_document(kvp("overall_distance", "$overall_distance"),
                          kvp("overall_duration", "$overall_duration"),
                          kvp("overall_elevation_gain", "$overall_elevation_gain"),
                          kvp("overall_elevation_loss", "$overall_elevation_loss"),
                          kvp("overall_count", "$overall_count"),
                          kvp("average_distance", make_document(kvp("$divide", make_array("$overall_distance","$overall_count")))),
                          kvp("average_pace", make_document(kvp("$cond", make_array(make_document(kvp("$eq", make_array("$overall_distance", 0))), 0.0, make_document(kvp("$divide", make_array("$overall_duration","$overall_distance")))))))

  ));
  p.sort(sorter.view());
  
  auto cursor = collection("sessions").aggregate(p, mongocxx::options::aggregate{});

  Output::print_track_based_stats(cursor, grouping); 
}

bool weekday_sort(std::pair<std::string, int>& a, std::pair<std::string, int>& b) {
  return (Helper::TimeConverter::weekday_to_idx(a.first) < Helper::TimeConverter::weekday_to_idx(b.first)); 
}

std::vector<std::pair<std::string, int>> MongoDB::build_day_vector(mongocxx::v_noabi::cursor& cursor) {
  std::vector<std::pair<std::string, int>> dayvec;
  
  for(auto doc : cursor) {
    int val = doc["count"].get_int32().value;

    std::string dayname = Helper::TimeConverter::mongo_idx_to_weekday_name(doc["_id"].get_int32().value);
    dayvec.push_back(std::make_pair(dayname, val));
  }
  std::sort(dayvec.begin(), dayvec.end(), weekday_sort);
 
  return dayvec;
}

/*
  db.sessions.aggregate([ 
    { $match: { year: 2021, month: 7, sport_type_id: 1 } }, 
    { $addFields: { weekday: { $dayOfWeek: "$start_time" } } }, 
    { $group: { _id: "$weekday", cnt: { $sum: 1 } } } 
  ])
*/
void MongoDB::aggregate_weekdays(std::vector<int> years, std::vector<int> sport_type_ids) {
  using namespace bsoncxx::builder::basic;
  mongocxx::pipeline p{};

  auto matcher = bsoncxx::builder::stream::document {};
  sport_type_matcher(matcher, sport_type_ids);
  year_matcher(matcher, years);

  p.match(matcher.view());
  p.add_fields(make_document(kvp("weekday", make_document(kvp("$dayOfWeek", "$start_time")))));
  p.group(make_document(kvp("_id", "$weekday"), 
                        kvp("count", make_document(kvp("$sum", 1)))
          ));
  p.sort(make_document(kvp("_id", 1)));

  auto cursor = collection("sessions").aggregate(p, mongocxx::options::aggregate{});

  auto dayvec = build_day_vector(cursor);

  Output::print_vector("Sessions per weekday", dayvec);
}


std::vector<std::pair<std::string, int>> MongoDB::build_hour_vector(mongocxx::v_noabi::cursor& cursor) {
  std::vector<std::pair<std::string, int>> hourvec;
  for(auto doc : cursor) {
    int val = doc["count"].get_int32().value;
    int hour = doc["_id"].get_int32().value;

    hourvec.push_back(make_pair(Helper::TimeConverter::hour_to_time_str(hour), val));
  }
  return hourvec;
}

/*
  db.sessions.aggregate([ 
    { $match: { year: 2021, month: 7, sport_type_id: 1 } }, 
    { $addFields: { hour: { $hour: "$start_time" } } }, 
    { $group: { _id: "$hour", cnt: { $sum: 1 } } } 
  ])
*/
void MongoDB::aggregate_hour_of_day(std::vector<int> years, std::vector<int> sport_type_ids) {
  using namespace bsoncxx::builder::basic;

  mongocxx::pipeline p{};

  auto matcher = bsoncxx::builder::stream::document {};
  sport_type_matcher(matcher, sport_type_ids);
  year_matcher(matcher, years);

  p.match(matcher.view());
  p.add_fields(make_document(kvp("hour", 
    make_document(kvp("$hour", 
      make_document(kvp("date", "$start_time"),kvp("timezone", "Europe/Vienna"))))
    )));
  p.group(make_document(kvp("_id", "$hour"), 
                        kvp("count", make_document(kvp("$sum", 1)))
          ));
  p.sort(make_document(kvp("_id", 1)));

  auto cursor = collection("sessions").aggregate(p, mongocxx::options::aggregate{});
  auto hourvec = build_hour_vector(cursor);

  Output::print_vector("Sessions per hour of day", hourvec);
}
