#pragma once

#include <string>

namespace Helper {

  class TimeConverter {

  public:

    static std::string time_to_string(const time_t time);
    static time_t date_time_string_to_time_t(const std::string time_str);
    static time_t string_to_time_t(const std::string);
    static std::string secs_to_min_str(uint32_t secs);
    static std::string ms_to_min_str(uint32_t ms);
    static std::string hour_to_time_str(uint32_t hour);
    static std::string mongo_idx_to_weekday_name(int i);
    static int weekday_to_idx(std::string name);

    static int time_str_to_ms(std::string time);
  };
}
