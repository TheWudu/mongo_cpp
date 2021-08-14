#pragma once

#include <vector>
#include <map>
#include <mongocxx/client.hpp>

#include "../models/session.hpp"
#include "../models/distance_bucket.hpp"
#include "../helper/time_converter.hpp"
#include "../helper/sport_types.hpp"

class Output {

  static std::string to_str(uint32_t val) {
    return std::to_string(val);
  }

public:

  static std::string meters_to_km(uint32_t meters) {
    float km = meters / 1000.0;
    std::stringstream ss;

    ss << std::fixed << std::setprecision(2) << km;
    return ss.str();
  }

  static void print_track_based_stats(mongocxx::v_noabi::cursor& cursor, std::vector<std::string> grouping);
  static void print_session_list(std::vector<Models::Session> sessions);
  static void print_vector(std::string title, std::vector<std::pair<std::string, int>> vec, std::string(*conv)(uint32_t) = &std::to_string);

  static void print_buckets(std::vector<DistanceBucket> const & buckets);
};
