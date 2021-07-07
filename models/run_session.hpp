#pragma once

#include <string>

namespace Models {
  class RunSession {

  public:

    std::string id;
    time_t      start_time;
    time_t      end_time;
    int32_t     start_time_timezone_offset;
    int32_t     distance;
    int32_t     duration;
    int32_t     sport_type_id;
    std::string notes;

    RunSession() {};

    void print();
    std::string time_str(time_t time);
  };
}
