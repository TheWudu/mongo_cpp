#pragma once

#include <string>

namespace Models {
  class RunSession {

  public:

    std::string id;
    time_t      start_time;
    time_t      end_time;
    int64_t     start_time_timezone_offset;
    int64_t     distance;
    int64_t     duration;
    int64_t     sport_type_id;
    std::string notes;

    RunSession() {};

    void print();
    std::string time_str(time_t time);
  };
}
