#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "models/weight.hpp"    
#include "models/session.hpp"
#include "mongo_db.hpp"

class Statistics {

private:
  template <class T>
  bsoncxx::builder::basic::array vector_to_array(std::vector<T> vec);

  static bool weekday_sort(std::pair<std::string, int>& a, std::pair<std::string, int>& b);

  void aggregate_basic_statistics(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<std::string> grouping);
  void aggregate_weekdays(std::vector<int> years, std::vector<int> sport_type_ids);
  void aggregate_hour_of_day(std::vector<int> years, std::vector<int> sport_type_ids);

  void sport_type_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> sport_type_ids);
  void year_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> years);

  std::vector<std::pair<std::string, int>> build_day_vector(mongocxx::v_noabi::cursor& cursor);
  std::vector<std::pair<std::string, int>> build_hour_vector(mongocxx::v_noabi::cursor& cursor);

public:

  void aggregate_stats(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<std::string> grouping);
};

