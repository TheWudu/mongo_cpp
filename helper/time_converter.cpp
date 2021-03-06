#include "time_converter.hpp"

#include <sstream>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>
#include <time.h>

int32_t Helper::TimeConverter::local_timezone_offset(const time_t time) {
  std::string local_time_str = Helper::TimeConverter::time_to_string(time);
  time_t local_time = Helper::TimeConverter::date_time_string_to_time_t(local_time_str);
  int32_t offset = local_time - time;

  return offset;
}

std::string Helper::TimeConverter::time_to_string(const time_t time) {
  char buf[25];
  struct tm * timeinfo = localtime(&time);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);

  return std::string { buf };
}
 
std::size_t replace_all(std::string& inout, std::string_view what, std::string_view with)
{
    std::size_t count{};
    for (std::string::size_type pos{};
         inout.npos != (pos = inout.find(what.data(), pos, what.length()));
         pos += with.length(), ++count) {
        inout.replace(pos, what.length(), with.data(), with.length());
    }
    return count;
}

void Helper::TimeConverter::set_timezone(std::string const timezone) {
  setenv("TZ", timezone.c_str(), 1);
  tzset();
}

std::string Helper::TimeConverter::get_timezone() {
  char* ctz = getenv("TZ");
  if(ctz != nullptr) {
    std::string tz { ctz };
    return tz;
  }
  return "Europe/Vienna";
}

time_t Helper::TimeConverter::date_time_string_to_time_t(std::string time_str) {
  replace_all(time_str, "T", " ");
  replace_all(time_str, "Z", "");
  boost::posix_time::ptime pt(boost::posix_time::time_from_string(time_str));
  static boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
  boost::posix_time::time_duration::sec_type secs = (pt - epoch).total_seconds();
  return time_t(secs);
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
    ss << std::setw(1) << std::setfill('0') << secs / 3600 << ":";
    secs %= 3600;
  }
  ss << std::setw(2) << std::setfill('0') << secs / 60 << ":"
     << std::setw(2) << std::setfill('0') << secs % 60
     << std::setfill(' ');
  return ss.str();
}

std::string Helper::TimeConverter::hour_to_time_str(uint32_t hour) {
  std::stringstream ss;
  ss << std::setw(2) << std::setfill('0') << hour << ":00" << std::setfill(' ');
  return ss.str();
}


/*
 Mongo starts with Sunday (1) and ends on Saturday (7)
*/
std::string Helper::TimeConverter::mongo_idx_to_weekday_name(int i) {
  static std::vector<std::string> weekday_strings = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
  };
  return weekday_strings.at(i - 1);
}

int Helper::TimeConverter::weekday_to_idx(std::string name) {
  static std::map<std::string, int> mapping = {
    { "Monday",    0 },
    { "Tuesday",   1 },
    { "Wednesday", 2 },
    { "Thursday",  3 },
    { "Friday",    4 },
    { "Saturday",  5 },
    { "Sunday",    6 }
  };
  return mapping[name];
}


int Helper::TimeConverter::time_str_to_ms(std::string time) {
  std::vector<std::string> strs;
  
  boost::split(strs, time, boost::is_any_of(":"));

  int ms = 0;
  for(auto s : strs) {
    ms *= 60;
    int v = std::stoi(s);
    ms += v;
  }
  return ms * 1000;
}
