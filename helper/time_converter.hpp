#pragma once

#include <string>

namespace Helper {

  class TimeConverter {

  public:

    static std::string time_to_string(time_t time);
  };
}
