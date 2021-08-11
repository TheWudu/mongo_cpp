#pragma once

#include <vector>
#include "../models/city.hpp"

class GeonamesParser {

public: 

  static GeonamesParser* instance() {
    if(_inst == nullptr) {
      _inst = new GeonamesParser();
    }
    return _inst;
  }

  ~GeonamesParser() {
    if(_inst != nullptr) {
      delete GeonamesParser::_inst;
    }

    for(auto city : cities) {
      delete city;
    }
  }

  std::string timezone_for(double const lat, double const lng);
  Models::City* nearest(double const lat, double const lng);

  void store_to_mongo();
  void parse_file(std::string filename);
  
private:
  
  GeonamesParser() {}

  static GeonamesParser* _inst; 
  std::vector<Models::City*> cities;

  void parse_default_file();

};
