#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility> // for std::pair
#include <cmath>

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
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include "../helper/time_converter.hpp"
#include "../helper/sport_types.hpp"
#include "../helper/output.hpp"
#include "../models/distance_bucket.hpp"

using namespace mongocxx;

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

#include "statistics.hpp"

void MongoDB::Statistics::aggregate_stats(std::vector<int> years, std::vector<int> sport_type_ids, 
                                 std::vector<std::string> grouping, std::vector<int> boundaries) {

  aggregate_basic_statistics(years, sport_type_ids, grouping);
  aggregate_years(years, sport_type_ids, grouping);
  aggregate_weekdays(years, sport_type_ids);
  aggregate_hour_of_day(years, sport_type_ids);
  aggregate_bucket_by_distance(years, sport_type_ids, boundaries);
}

bool MongoDB::Statistics::weekday_sort(std::pair<std::string, int>& a, std::pair<std::string, int>& b) {
  return (Helper::TimeConverter::weekday_to_idx(a.first) < Helper::TimeConverter::weekday_to_idx(b.first)); 
}

bool MongoDB::Statistics::year_sort(std::pair<std::string, int>& a, std::pair<std::string, int>& b) {
  return (std::stoi(a.first) < std::stoi(b.first)); 
}

bsoncxx::builder::stream::document MongoDB::Statistics::base_matcher(std::vector<int> years, std::vector<int> sport_type_ids) {
  bsoncxx::builder::stream::document matcher;
  
  sport_type_matcher(matcher, sport_type_ids);
  year_matcher(matcher, years);

  return matcher;
}

/* 
  db.sessions.aggregate([ 
    { $match: { "sport_type_id": { $in: [1,3,4,19] }, year: { $in: [2021,2020] } } }, 
    { $group: { _id: "$sport_type_id", overall_distance: { $sum: "$distance" }, overall_duration: { $sum: "$duration" }, overall_count: { $sum: 1 } } }, 
    { $project: { overall_distance: "$overall_distance", overall_duration: "$overall_duration", overall_count: "$overall_count", avg_distance: { $divide: [ "$overall_distance", "$overall_count" ] }, average_pace: { $divide: [ "$overall_duration", "$overall_distance"] } } } 
  ])
*/
void MongoDB::Statistics::aggregate_basic_statistics(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<std::string> grouping) {
  using namespace bsoncxx::builder::basic;

  mongocxx::pipeline p{};
  auto matcher = base_matcher(years, sport_type_ids); 

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
  
  auto cursor = collection(this->client, "sessions").aggregate(p, mongocxx::options::aggregate{});

  Output::print_track_based_stats(cursor, grouping); 
}


/*
  db.sessions.aggregate([ 
    { $match: { year: { $in: [2021] }, sport_type_id: { $in: [1] } } }, 
    { $group: { _id: "$year", 
        overall_distance: { $sum: "$distance" },
        overall_duration: { $sum: "$duration" },
        overall_elevation_gain: { $sum: "$elevation_gain" },
        overall_cnt: { $sum: 1 } } },
    { $sort: { _id: 1 } }
  ])
*/
void MongoDB::Statistics::aggregate_years(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<std::string> grouping) {
  using namespace bsoncxx::builder::basic;
  mongocxx::pipeline p{};

  auto matcher = base_matcher(years, sport_type_ids);
  
  auto group_by = bsoncxx::builder::stream::document {};
  for(auto g : grouping) {
    group_by << g << "$" + g;
  }

  p.match(matcher.view());
  p.group(make_document(kvp("_id", group_by.view()), 
                        kvp("overall_distance", make_document(kvp("$sum", "$distance"))),
                        kvp("overall_duration", make_document(kvp("$sum", "$duration"))),
                        kvp("overall_elevation_gain", make_document(kvp("$sum", "$elevation_gain"))),
                        kvp("overall_count", make_document(kvp("$sum", 1)))
          ));
  p.sort(make_document(kvp("_id", 1)));

  auto cursor = collection(this->client, "sessions").aggregate(p, mongocxx::options::aggregate{});

  std::vector<std::string> attrs = std::vector { 
    std::string("overall_distance"), 
    std::string("overall_duration"),
    std::string("overall_elevation_gain")
  };
  auto vecs = build_vectors(cursor, attrs);

  Output::print_vector("Distance", vecs.at(0), &Output::meters_to_km);
  Output::print_vector("Duration", vecs.at(1), &Helper::TimeConverter::ms_to_min_str);
  Output::print_vector("Elevation gain", vecs.at(2));
}


/*
  db.sessions.aggregate([ 
    { $match: { year: 2021, month: 7, sport_type_id: 1 } }, 
    { $addFields: { weekday: { $dayOfWeek: "$start_time" } } }, 
    { $group: { _id: "$weekday", cnt: { $sum: 1 } } },
    { $sort: { _id: 1 } }
  ])
*/
void MongoDB::Statistics::aggregate_weekdays(std::vector<int> years, std::vector<int> sport_type_ids) {
  using namespace bsoncxx::builder::basic;
  mongocxx::pipeline p{};

  auto matcher = base_matcher(years, sport_type_ids);

  p.match(matcher.view());
  p.add_fields(make_document(kvp("weekday", make_document(kvp("$dayOfWeek", "$start_time")))));
  p.group(make_document(kvp("_id", "$weekday"), 
                        kvp("count", make_document(kvp("$sum", 1)))
          ));
  p.sort(make_document(kvp("_id", 1)));

  auto cursor = collection(this->client, "sessions").aggregate(p, mongocxx::options::aggregate{});

  auto dayvec = build_day_vector(cursor);

  Output::print_vector("Sessions per weekday", dayvec);
}

/*
  db.sessions.aggregate([ 
    { $match: { year: { $in: [2021] }, sport_type_id: { $in: [1] } } }, 
    { $addFields: { timezone: { $ifNull: [ "$timezone", "UTC" ] } } }
    { $addFields: { hour: { $hour: { date: "$start_time", timezone: "$timezone" } } } }, 
    { $group: { _id: "$hour", count: { $sum: 1 } } },
    { $sort: { _id: 1 } }
  ])
*/
void MongoDB::Statistics::aggregate_hour_of_day(std::vector<int> years, std::vector<int> sport_type_ids) {
  using namespace bsoncxx::builder::basic;

  mongocxx::pipeline p{};

  auto matcher = base_matcher(years, sport_type_ids); 

  p.match(matcher.view());
  p.add_fields(make_document(kvp("timezone", 
    make_document(kvp("$ifNull", 
      make_array("$timezone", "UTC"))))
    ));
  p.add_fields(make_document(kvp("hour", 
    make_document(kvp("$hour", 
      make_document(kvp("date", "$start_time"),kvp("timezone", "$timezone"))))
    )));
  p.group(make_document(kvp("_id", "$hour"), 
                        kvp("count", make_document(kvp("$sum", 1)))
          ));
  p.sort(make_document(kvp("_id", 1)));

  auto cursor = collection(this->client, "sessions").aggregate(p, mongocxx::options::aggregate{});
  auto hourvec = build_hour_vector(cursor);

  Output::print_vector("Sessions per hour of day", hourvec);
}

/*
  db.sessions.aggregate([
    { $match: { year: { $in: [2020,2021] }, sport_type_id: 1 } }, 
    { $bucket: { 
        groupBy: "$distance", 
        boundaries: [0,5000,10000,20000,Infinity], 
        output: { 
          total: { $sum: 1 }, 
          avg_distance: { $avg: "$distance" }, 
          sum_distance: { $sum: "$distance" }, 
          sum_duration: { $sum: "$duration" } 
        } 
      } 
    }
  ])
*/
void MongoDB::Statistics::aggregate_bucket_by_distance(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<int> boundaries) {
  using namespace bsoncxx::builder::basic;

  mongocxx::pipeline p{};

  auto matcher = base_matcher(years, sport_type_ids); 
  if(boundaries.front() != 0) {
    boundaries.insert(boundaries.begin(),0);
  }
  if(boundaries.back() < std::numeric_limits<int>::max()) {
    boundaries.push_back(std::numeric_limits<int>::max());
  }

  p.match(matcher.view());
  p.bucket(make_document(
    kvp("groupBy", "$distance"),
    kvp("boundaries", MongoDB::Base::vector_to_array(boundaries)),
    kvp("output", make_document(
      kvp("total", make_document(kvp("$sum", 1))),
      kvp("avg_distance", make_document(kvp("$avg", "$distance"))),
      kvp("sum_distance", make_document(kvp("$sum", "$distance"))),
      kvp("sum_duration", make_document(kvp("$sum", "$duration")))
    ))));

  auto cursor = collection(this->client, "sessions").aggregate(p, mongocxx::options::aggregate{});


  auto buckets = build_distance_buckets_vector(cursor, boundaries);

  Output::print_buckets(buckets);
}

/*
  Vector building methods
*/
std::vector<DistanceBucket> MongoDB::Statistics::build_distance_buckets_vector(mongocxx::v_noabi::cursor& cursor, std::vector<int> boundaries) {
  std::vector<DistanceBucket> buckets;

  for(auto doc : cursor) {
    uint32_t upper_bound = 0;
    uint32_t lower_bound = doc["_id"].get_int32().value;
    for(auto p = boundaries.begin(); p != boundaries.end(); p++) {
      if(*p == (int)lower_bound) {
        upper_bound = *(++p);
      }
    }

    DistanceBucket bucket;
    bucket.lower_bound = lower_bound;
    bucket.upper_bound = upper_bound;
    bucket.total       = doc["total"].get_int32().value;
    bucket.avg_dist    = std::round(doc["avg_distance"].get_double().value);
    bucket.sum_dist    = doc["sum_distance"].get_int32().value;
    bucket.sum_dur     = doc["sum_duration"].get_int32().value;

    buckets.push_back(bucket);
  }
  return buckets;
}

std::vector<std::pair<std::string, int>> MongoDB::Statistics::build_hour_vector(mongocxx::v_noabi::cursor& cursor) {
  std::vector<std::pair<std::string, int>> hourvec;
  for(auto doc : cursor) {
    // std::cout << bsoncxx::to_json(doc) << std::endl;
    int val = doc["count"].get_int32().value;
    int hour = doc["_id"].get_int32().value;

    hourvec.push_back(std::make_pair(Helper::TimeConverter::hour_to_time_str(hour), val));
  }
  return hourvec;
}

std::vector<std::pair<std::string, int>> MongoDB::Statistics::build_day_vector(mongocxx::v_noabi::cursor& cursor) {
  std::vector<std::pair<std::string, int>> dayvec;
  
  for(auto doc : cursor) {
    int val = doc["count"].get_int32().value;

    std::string dayname = Helper::TimeConverter::mongo_idx_to_weekday_name(doc["_id"].get_int32().value);
    dayvec.push_back(std::make_pair(dayname, val));
  }
  std::sort(dayvec.begin(), dayvec.end(), weekday_sort);
 
  return dayvec;
}

std::vector<std::vector<std::pair<std::string, int>>> MongoDB::Statistics::build_vectors(mongocxx::v_noabi::cursor& cursor, std::vector<std::string> attrs) {
  std::vector<std::vector<std::pair<std::string, int>>> vecs (attrs.size());
  
  for(auto doc : cursor) {
    for(uint32_t i = 0; i < attrs.size(); i++) {

      int val = doc[attrs[i]].get_int32().value;

      auto d = doc["_id"].get_document();
      std::stringstream ss;
      for (bsoncxx::document::element ele : d.view()) {
        if( ss.tellp() != 0) {
          ss << "-";
        }
        // element is non owning view of a key-value pair within a document.

        // we can use the key() method to get a string_view of the key.
        // stdx::string_view field_key{ele.key()};
        // std::cout << "Got key, key = " << field_key << std::endl;
        // std::cout << std::to_string(ele.get_int32().value) << std::endl;
        ss << std::to_string(ele.get_int32().value);
      }
      // std::cout << bsoncxx::to_json(d) << std::endl;
      // std::string name = std::to_string(doc["_id"].get_int32().value);
      std::string name = ss.str();
      vecs.at(i).push_back(std::make_pair(name, val));
    }
  }
 
  return vecs;
}
