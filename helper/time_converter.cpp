#include "time_converter.hpp"


std::string Helper::TimeConverter::time_to_string(time_t time) {
  char buf[19];
  struct tm * timeinfo = localtime(&time);
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H-%MZ", timeinfo);

  return std::string { buf };
}
