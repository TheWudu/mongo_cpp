#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "gpx_parser.hpp"

void GpxParser::parse_file(std::string const filename) {
  std::string line;
  std::ifstream filestream(filename);
  std::vector<gpx_tags> state;

 
  this->data.clear();
  std::cout << "GpxFile: " << filename << std::endl;

  while (getline (filestream, line)) {
    std::cout << line << std::endl;

    if(state.size() != 0) {
      switch(state.back()) {
        case(gpx):    parse_state_gpx(line, state);    break;
        case(trk):    parse_state_trk(line, state);    break;
        case(trkseg): parse_state_trkseg(line, state); break;
        case(trkpt):  parse_state_trkpt(line, state);  break;
        default: break;
      }
    }

    if(line.find("<gpx") != std::string::npos) {
      state.push_back(gpx);
    }
  }

  for(GpxPoint* gpx : this->data) {
    std::cout << gpx->lat << ", " << gpx->lng << " - " << gpx->elevation << std::endl;
  }
  std::cout << std::endl;
}

void GpxParser::parse_state_gpx(std::string line, std::vector<gpx_tags>& state) {
  std::cout << "parse state gpx: " << line << std::endl;

  if(line.find("<trk>") != std::string::npos) {
std::cout << "found trk: " << line.find("<trk>") << std::endl;
    state.push_back(trk);
  }

  if(line.find("</gpx>") != std::string::npos) {
std::cout << "pop back" << std::endl;
    state.pop_back();
  }
}

void GpxParser::parse_state_trk(std::string line, std::vector<gpx_tags>& state) {
  std::cout << "parse state trk" << std::endl;
  if(line.find("<trkseg>") != std::string::npos) {
    state.push_back(trkseg);
  }

  if(line.find("</trk>") != std::string::npos) {
    state.pop_back();
  }
}

void GpxParser::get_double_value(std::string s, std::string attr, double& val) {
  if(s.find(attr) != std::string::npos) {
    val = std::stod(s.substr(s.find("=") + 2, s.size() - s.find("=") - 3));
  }
}


void GpxParser::parse_state_trkseg(std::string line, std::vector<gpx_tags>& state) {
  std::vector<std::string> strs;
  std::cout << "parse state trkseg" << std::endl;
    
  this->lat = 0.0;
  this->lng = 0.0;

  if(line.find("<trkpt") != std::string::npos) {
    state.push_back(trkpt);

    boost::trim(line);
    boost::split(strs, line, boost::is_any_of(" "));

    for(auto s : strs) {
      std::cout << "  " << s << std::endl;
      get_double_value(s, "lat", this->lat);
      get_double_value(s, "lon", this->lng);
    }
      
    std::cout << "Lat/Lng: " << lat << "/" << lng << std::endl;
  }

  if(line.find("</trkseg>") != std::string::npos) {
    state.pop_back();
  }
}
void GpxParser::parse_state_trkpt(std::string line, std::vector<gpx_tags>& state) {

  std::cout << "parse state trkpt" << std::endl;
  
  auto pos = line.find("<ele>");
  if(pos != std::string::npos) {
    this->elevation = std::stod(line.substr(pos + 5, line.find("</ele>")));
    
    std::cout << "Elevation: " << std::setprecision(5) << this->elevation << std::endl;
  }

  if(line.find("</trkpt>") != std::string::npos) {

    GpxPoint* gpx_point = new GpxPoint();
    gpx_point->lat       = this->lat;
    gpx_point->lng       = this->lng;
    gpx_point->elevation = this->elevation;
    std::cout << gpx_point->lat << ", " << gpx_point->lng << " - " << gpx_point->elevation << std::endl;
   
    std::cout << "data.size(): " << data.size() << std::endl; 
    
    this->data.push_back(gpx_point);

    state.pop_back();
  }
}
