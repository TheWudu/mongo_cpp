#include <iostream>

#include "weight.hpp"

  Weight::Weight(std::string id, time_t date, float weight) {
    this->id = id;
    this->date = date;
    this->weight = weight;
  }

  void Weight::print() {
    std::cout << "id: " << this->id 
      << " - " << time_str(this->date) << " - " << this->weight << "\n";
  }

  std::string Weight::time_str(time_t time) {
    char buf[19];
    struct tm * timeinfo = localtime(&time);
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H-%MZ", timeinfo);

    return std::string { buf };
  }

