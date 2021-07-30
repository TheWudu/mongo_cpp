#pragma once

#include <vector>
#include <string>
#include "helper/time_converter.hpp"


enum gpx_tags {
  gpx,
  trk,
  trkseg,
  trkpt,
  ele
};

class GpxPoint {

public:

  double lat;
  double lng;
  double elevation;
  time_t time;

  GpxPoint() {};
  GpxPoint(const GpxPoint &p1) { 
    lat = p1.lat; 
    lng = p1.lng; 
    elevation = p1.elevation; 
    time = p1.time;
  }

  void print() {
    std::cout << this->lat << ", " << this->lng << " - " << this->elevation << " - " << Helper::TimeConverter::time_to_string(this->time) << std::endl;
  }
};

class GpxParser {

public:

  void parse_file(std::string filename);

  ~GpxParser () {
    for(auto gpx_point : data) {
      delete gpx_point;
    } 
  }

private:

  double lat;
  double lng;
  double elevation;
  time_t time;

  std::vector<GpxPoint*> data;

  void parse_state_gpx(std::string line, std::vector<gpx_tags>& state);
  void parse_state_trk(std::string line, std::vector<gpx_tags>& state);
  void parse_state_trkseg(std::string line, std::vector<gpx_tags>& state);
  void parse_state_trkpt(std::string line, std::vector<gpx_tags>& state);

  void get_double_value(std::string line, std::string attr, double& val);
  void get_string_in_tags(std::string s, std::string attr, std::string& val);
};

