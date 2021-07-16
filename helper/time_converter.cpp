#include "time_converter.hpp"

#include <sstream>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

std::string Helper::TimeConverter::time_to_string(const time_t time) {
  char buf[19];
  struct tm * timeinfo = localtime(&time);
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H-%MZ", timeinfo);

  return std::string { buf };
}

time_t Helper::TimeConverter::string_to_time_t( const std::string time_str ) {

  boost::gregorian::date date = boost::gregorian::from_simple_string(time_str);
  static boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
  boost::posix_time::time_duration::sec_type secs = (boost::posix_time::ptime(date,boost::posix_time::seconds(0)) - epoch).total_seconds();
  time_t time = time_t(secs);

  return time;
}

std::string Helper::TimeConverter::ms_to_min_str( uint32_t ms ) {
  return secs_to_min_str(ms / 1000);
}

std::string Helper::TimeConverter::secs_to_min_str( uint32_t secs ) {
  std::stringstream ss;

  if(secs > 3600) {
    ss << secs / 3600 << ":";
    secs %= 3600;
  }
  ss << std::setw(2) << std::setfill('0') << secs / 60 << ":"
     << std::setw(2) << std::setfill('0') << secs % 60
     << std::setfill(' ');
  return ss.str();
}


std::string Helper::TimeConverter::weekday_name(int i) {
  static std::vector<std::string> weekday_strings = {
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
  };
  return weekday_strings.at(i);
}

