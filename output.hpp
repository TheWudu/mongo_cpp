#pragma once

#include <vector>
#include <map>
#include <mongocxx/client.hpp>

#include "models/session.hpp"
#include "helper/time_converter.hpp"

class Output {

public:
  static void print_track_based_stats(mongocxx::v_noabi::cursor& cursor, std::vector<std::string> grouping);
  static void print_session_list(std::vector<Models::Session> sessions);
  static void print_vector(std::string title, std::vector<std::pair<std::string, int>> vec);
};
