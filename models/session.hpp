#pragma once

#include <string>

namespace Models {
  class Session {

  public:

    std::string id;
    time_t      start_time;
    time_t      end_time;
    int32_t     start_time_timezone_offset;
    int32_t     distance;
    int32_t     duration;
    int32_t     pause;
    int32_t     elevation_gain;
    int32_t     elevation_loss;
    int32_t     sport_type_id;
    std::string notes;

    Session() {};

    void print();
  };
}
