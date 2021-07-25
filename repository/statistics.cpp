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

#include "statistics.hpp"

template <class T>
bsoncxx::builder::basic::array Statistics::vector_to_array(std::vector<T> vec) {
  bsoncxx::builder::basic::array a = bsoncxx::builder::basic::array();
  for(auto v : vec) {
    a.append(v);
  }
  return a;
}

void Statistics::aggregate_stats(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<std::string> grouping) {

  aggregate_basic_statistics(years, sport_type_ids, grouping);
  aggregate_weekdays(years, sport_type_ids);
  aggregate_hour_of_day(years, sport_type_ids);
}

void Statistics::sport_type_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> sport_type_ids) {
  using namespace bsoncxx::builder::basic;
  if(sport_type_ids.size() > 0) {
    matcher << "sport_type_id" << open_document <<
      "$in" << vector_to_array(sport_type_ids) << close_document;
  }
}
  
void Statistics::year_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> years) {
  if(years.size() > 0) {
    matcher << "year" << open_document <<
      "$in" << vector_to_array(years) << close_document;
  }
}
 
/* 
  db.sessions.aggregate([ 
    { $match: { "sport_type_id": { $in: [1,3,4,19] }, "start_time": { $gt: ISODate("2021-04-01"), $lt: ISODate("2022-01-01") } } }, 
    { $group: { _id: "$sport_type_id", overall_distance: { $sum: "$distance" }, overall_duration: { $sum: "$duration" }, overall_count: { $sum: 1 } } }, 
    { $project: { overall_distance: "$overall_distance", overall_duration: "$overall_duration", overall_count: "$overall_count", avg_distance: { $divide: [ "$overall_distance", "$overall_count" ] }, average_pace: { $divide: [ "$overall_duration", "$overall_distance"] } } } 
  ] years
*/
void Statistics::aggregate_basic_statistics(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<std::string> grouping) {
  using namespace bsoncxx::builder::basic;

  mongocxx::pipeline p{};
  auto matcher = bsoncxx::builder::stream::document {};
  sport_type_matcher(matcher, sport_type_ids);
  year_matcher(matcher, years);

  auto group_by = bsoncxx::builder::stream::document {};
  auto sorter   = bsoncxx::builder::stream::document {};
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
  
  MongoDB* mc = MongoDB::connection();
  auto cursor = mc->collection("sessions").aggregate(p, mongocxx::options::aggregate{});

  Output::print_track_based_stats(cursor, grouping); 
}

bool Statistics::weekday_sort(std::pair<std::string, int>& a, std::pair<std::string, int>& b) {
  return (Helper::TimeConverter::weekday_to_idx(a.first) < Helper::TimeConverter::weekday_to_idx(b.first)); 
}

std::vector<std::pair<std::string, int>> Statistics::build_day_vector(mongocxx::v_noabi::cursor& cursor) {
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
void Statistics::aggregate_weekdays(std::vector<int> years, std::vector<int> sport_type_ids) {
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

  MongoDB* mc = MongoDB::connection();
  auto cursor = mc->collection("sessions").aggregate(p, mongocxx::options::aggregate{});

  auto dayvec = build_day_vector(cursor);

  Output::print_vector("Sessions per weekday", dayvec);
}


std::vector<std::pair<std::string, int>> Statistics::build_hour_vector(mongocxx::v_noabi::cursor& cursor) {
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
void Statistics::aggregate_hour_of_day(std::vector<int> years, std::vector<int> sport_type_ids) {
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

  MongoDB* mc = MongoDB::connection();
  auto cursor = mc->collection("sessions").aggregate(p, mongocxx::options::aggregate{});
  auto hourvec = build_hour_vector(cursor);

  Output::print_vector("Sessions per hour of day", hourvec);
}
