#pragma once

#include <string>

namespace Helper {

  class TimeConverter {

  public:

    static std::string time_to_string(time_t time);
    static time_t string_to_time_t(const std::string);
  };
}
