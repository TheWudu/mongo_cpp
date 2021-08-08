#pragma once

#include <vector>

class City {
public:
  double lat;
  double lng;
  std::string name;
  std::string timezone; 
};

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
  City* nearest(double const lat, double const lng);
  
private:
  
  GeonamesParser() {
    parse_file();
  }

  static GeonamesParser* _inst; 
  std::vector<City*> cities;

  void parse_file();

};
