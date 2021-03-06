#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "../models/weight.hpp"    
#include "../models/session.hpp"
#include "../models/distance_bucket.hpp"
#include "base.hpp"

namespace MongoDB {
  class Statistics : public Base {

  public:

    Statistics() : client( MongoConnection::connection()->client() ) {
    }

    void aggregate_stats(std::vector<int> years, 
                         std::vector<int> sport_type_ids, 
                         std::vector<std::string> grouping,
                         std::vector<int> boundaries);
  private:
    mongocxx::pool::entry client;

    static bool weekday_sort(std::pair<std::string, int>& a, std::pair<std::string, int>& b);
    static bool year_sort(std::pair<std::string, int>& a, std::pair<std::string, int>& b);
    
    std::vector<std::pair<std::string, int>> build_day_vector(mongocxx::v_noabi::cursor& cursor);
    std::vector<std::pair<std::string, int>> build_hour_vector(mongocxx::v_noabi::cursor& cursor);
    std::vector<std::vector<std::pair<std::string, int>>> build_vectors(mongocxx::v_noabi::cursor& cursor, std::vector<std::string> attrs);
    std::vector<DistanceBucket> build_distance_buckets_vector(mongocxx::v_noabi::cursor& cursor, std::vector<int> boundaries);

    bsoncxx::builder::stream::document base_matcher(std::vector<int> years, std::vector<int> sport_type_ids);

    void aggregate_basic_statistics(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<std::string> grouping);
    void aggregate_weekdays(std::vector<int> years, std::vector<int> sport_type_ids);
    void aggregate_hour_of_day(std::vector<int> years, std::vector<int> sport_type_ids);
    void aggregate_years(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<std::string> grouping);

    void aggregate_bucket_by_distance(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<int> boundaries);
  };
}
