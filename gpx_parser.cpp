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
    gpx->print();
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

void GpxParser::get_string_in_tags(std::string s, std::string attr, std::string& val) {

  std::string tag = "<" + attr + ">";
  std::string etag = "</" + attr + ">";
  if(s.find(tag) != std::string::npos) {
    std::cout << s.find(tag) << " - " << attr.size() + 2 << " - " << s.size() << " - " << s.find(etag) << std::endl;
    size_t begin = s.find(tag) + attr.size() + 2;
    size_t len   = s.find(etag) - begin;
    val = s.substr(begin, len);
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
    this->elevation = std::stod(line.substr(pos + 5, line.find("</ele>") - 6));
    
    std::cout << "Elevation: " << std::setprecision(5) << this->elevation << std::endl;
  }
  
  pos = line.find("<time>");
  if(pos != std::string::npos) {
    std::string time_str;
    std::string tag("time");
    get_string_in_tags(line, tag, time_str);

    std::cout << "Time: " << time_str << std::endl;
    this->time = Helper::TimeConverter::date_time_string_to_time_t(time_str);
    
    std::cout << "Time: "  << this->time << " == " << Helper::TimeConverter::time_to_string(this->time) <<  std::endl;
  }

  if(line.find("</trkpt>") != std::string::npos) {

    GpxPoint* gpx_point = new GpxPoint();
    gpx_point->lat       = this->lat;
    gpx_point->lng       = this->lng;
    gpx_point->elevation = this->elevation;
    gpx_point->time      = this->time;
    gpx_point->print();
 
    std::cout << "data.size(): " << data.size() << std::endl; 
    
    this->data.push_back(gpx_point);

    state.pop_back();
  }
}
