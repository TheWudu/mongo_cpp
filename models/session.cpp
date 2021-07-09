#include <iostream>

#include "../helper/time_converter.hpp"
#include "session.hpp"

namespace Models {

  void Session::print() {
    std::cout << "id: " << this->id 
      << " - " << this->sport_type_id
      << " - " << Helper::TimeConverter::time_to_string(this->start_time) 
      << " - " << Helper::TimeConverter::time_to_string(this->end_time) 
      << " - " << this->duration 
      << " - " << this->distance
      << " - " << this->notes
      << std::endl;
  }
}
