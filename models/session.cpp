#include <iostream>

#include "../helper/time_converter.hpp"
#include "../helper/sport_types.hpp"
#include "session.hpp"

namespace Models {

  void Session::print() {
    std::cout << "Session (id: " << this->id << ")" << std::endl
      << "  sport type:    " << Helper::SportType::name(this->sport_type_id) << std::endl
      << "  start time:    " << Helper::TimeConverter::time_to_string(this->start_time) << std::endl
      << "  end time:      " << Helper::TimeConverter::time_to_string(this->end_time) << std::endl
      << "  duration:      " << this->duration << std::endl
      << "  distance:      " << this->distance << std::endl
      << "  elevation_gain " << this->elevation_gain << std::endl
      << "  elevation_loss " << this->elevation_loss << std::endl
      << "  notes:         " << this->notes << std::endl
      << std::endl;
  }
}
