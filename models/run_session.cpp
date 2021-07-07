#include <iostream>

#include "run_session.hpp"

namespace Models {

  void RunSession::print() {
    std::cout << "id: " << this->id 
      << " - " << time_str(this->start_time) 
      << " - " << time_str(this->end_time) 
      << " - " << this->duration 
      << " - " << this->distance
      << std::endl;
  }

  std::string RunSession::time_str(time_t time) {
    char buf[19];
    struct tm * timeinfo = localtime(&time);
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H-%MZ", timeinfo);

    return std::string { buf };
  }
}
