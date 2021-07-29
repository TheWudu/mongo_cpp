#pragma once

#include <vector>
#include <string>


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

  GpxPoint() {};
  GpxPoint(const GpxPoint &p1) { lat = p1.lat; lng = p1.lng; elevation = p1.elevation; }
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

  std::vector<GpxPoint*> data;

  void parse_state_gpx(std::string line, std::vector<gpx_tags>& state);
  void parse_state_trk(std::string line, std::vector<gpx_tags>& state);
  void parse_state_trkseg(std::string line, std::vector<gpx_tags>& state);
  void parse_state_trkpt(std::string line, std::vector<gpx_tags>& state);

  void get_double_value(std::string line, std::string attr, double& val);
};

