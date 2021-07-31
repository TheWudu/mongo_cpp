#pragma once

#include <vector>
#include <string>
#include "helper/time_converter.hpp"
#include "models/session.hpp"


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
    std::cout << std::setprecision(16) << this->lat << ", " 
              << std::setprecision(16) << this->lng << " - " 
              << this->elevation << " - " 
              << Helper::TimeConverter::time_to_string(this->time) << std::endl;
  }
};

class GpxParser {

public:

  void parse_file(std::string filename);
  Models::Session* build_model();

  ~GpxParser () {
    for(auto gpx_point : data) {
      delete gpx_point;
    } 
  }

private:

  // per gpx trkpoint 
  double lat;
  double lng;
  double elevation;
  time_t time;
  // per gpx trace (only once)
  std::string type;
  std::string name; 
  double distance;
  double elevation_gain;
  double elevation_loss;
  time_t start_time;
  time_t end_time;
  uint32_t duration;

  std::vector<GpxPoint*> data;

  void calculate_stats();

  void parse_state_gpx(std::string line, std::vector<gpx_tags>& state);
  void parse_state_trk(std::string line, std::vector<gpx_tags>& state);
  void parse_state_trkseg(std::string line, std::vector<gpx_tags>& state);
  void parse_state_trkpt(std::string line, std::vector<gpx_tags>& state);

  void get_double_value(std::string line, std::string attr, double& val);
  void get_string_in_tags(std::string s, std::string attr, std::string& val);
};

