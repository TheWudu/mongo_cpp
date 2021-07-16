#pragma once

#include <vector>

#include "models/session.hpp"

class Output {

public:
  static void print_session_list(std::vector<Models::Session> sessions);
};
