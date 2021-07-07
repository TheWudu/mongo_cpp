#include <iostream>

#include "../helper/time_converter.hpp"
#include "weight.hpp"

namespace Models {
  Weight::Weight(std::string id, time_t date, float weight) {
    this->id = id;
    this->date = date;
    this->weight = weight;
  }

  void Weight::print() {
    std::cout << "id: " << this->id 
      << " - " << Helper::TimeConverter::time_to_string(this->date) << " - " << this->weight << "\n";
  }

}
