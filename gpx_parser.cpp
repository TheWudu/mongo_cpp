#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "helper/geokit.hpp"
#include "helper/sport_types.hpp"
#include "../models/session.hpp"
#include "repository/mongo_db.hpp"

#include "gpx_parser.hpp"

void GpxParser::parse_file(std::string const filename) {
  std::string line;
  std::ifstream filestream(filename);
  std::vector<gpx_tags> state;

  this->data.clear();

  while (getline (filestream, line)) {
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
}

void GpxParser::calculate_stats() {
  double elevation_gain = 0.0;
  double elevation_loss = 0.0;
  double distance       = 0.0;

  time_t start_time = data.front()->time;
  time_t end_time   = data.back()->time;
  uint32_t duration = (end_time - start_time) * 1000;
  uint32_t pause    = 0;

  GpxPoint* pp = (*data.begin());
  
  for(GpxPoint* p : data) {
    double diff = (p->elevation - pp->elevation);
    if(diff >= 0.0) {
      elevation_gain += diff;
    }
    else {
      elevation_loss += std::abs(diff);
    }

    auto dur_diff = p->time - pp->time;
    if(dur_diff >= 20) {
      pause += dur_diff;
    }

    distance += Helper::Geokit::distance_earth(pp->lat, pp->lng, p->lat, p->lng);
    pp = p;
  }
  distance *= 1000.0;

  this->distance = distance;
  this->elevation_gain = elevation_gain;
  this->elevation_loss = elevation_loss;
  this->start_time = start_time;
  this->end_time = end_time;
  this->duration = duration - pause * 1000;
  this->pause    = pause;
  
  // std::cout << "Start time:     " << Helper::TimeConverter::time_to_string(start_time) << std::endl
  //           << "End time:       " << Helper::TimeConverter::time_to_string(end_time) << std::endl
  //           << "Duration:       " << Helper::TimeConverter::ms_to_min_str(duration) << std::endl
  //           << "Distance:       " << (uint32_t)distance << " [m]" << std::endl
  //           << "Elevation_gain: " << (uint32_t)elevation_gain << std::endl
  //           << "Elevation_loss: " << (uint32_t)elevation_loss << std::endl;
}
  
Models::Session* GpxParser::build_model() {
  calculate_stats();

  Models::Session* session = new Models::Session;

  session->id             = MongoDB::new_object_id();
  session->sport_type_id  = Helper::SportType::id(this->type); 
  session->distance       = (uint32_t)(this->distance);
  session->duration       = this->duration;
  session->elevation_gain = this->elevation_gain;
  session->elevation_loss = this->elevation_loss;
  session->start_time_timezone_offset = Helper::TimeConverter::local_timezone_offset(this->start_time);
  session->start_time     = this->start_time;
  session->end_time       = this->end_time;
  session->notes          = this->name; 
  session->pause          = this->pause;

  // session->print();

  return session;
}

void GpxParser::parse_state_gpx(std::string line, std::vector<gpx_tags>& state) {
  // std::cout << "parse state gpx: " << line << std::endl;

  if(line.find("<trk>") != std::string::npos) {
    state.push_back(trk);
  }

  if(line.find("</gpx>") != std::string::npos) {
    state.pop_back();
  }
}

void GpxParser::parse_state_trk(std::string line, std::vector<gpx_tags>& state) {
  // std::cout << "parse state trk" << std::endl;
 
  get_string_in_tags(line, "type", this->type);
  get_string_in_tags(line, "name", this->name);

  if(line.find("<trkseg>") != std::string::npos) {
    state.push_back(trkseg);
  }

  if(line.find("</trk>") != std::string::npos) {
    state.pop_back();
  }
}

void GpxParser::get_double_value(std::string s, std::string attr, double& val) {
  if(s.find(attr) != std::string::npos) {
    std::string str = s.substr(s.find("=") + 2, s.size() - s.find("=") - 3); 
    val = std::stod(str);
  }
}

void GpxParser::get_string_in_tags(std::string s, std::string attr, std::string& val) {

  std::string tag = "<" + attr + ">";
  std::string etag = "</" + attr + ">";
  if(s.find(tag) != std::string::npos) {
    // std::cout << s.find(tag) << " - " << attr.size() + 2 << " - " << s.size() << " - " << s.find(etag) << std::endl;
    size_t begin = s.find(tag) + attr.size() + 2;
    size_t len   = s.find(etag) - begin;
    val = s.substr(begin, len);
  }
}


void GpxParser::parse_state_trkseg(std::string line, std::vector<gpx_tags>& state) {
  std::vector<std::string> strs;
  // std::cout << "parse state trkseg" << std::endl;
    
  this->lat = 0.0;
  this->lng = 0.0;

  if(line.find("<trkpt") != std::string::npos) {
    state.push_back(trkpt);

    boost::trim(line);
    boost::split(strs, line, boost::is_any_of(" "));

    for(auto s : strs) {
      get_double_value(s, "lat", this->lat);
      get_double_value(s, "lon", this->lng);
    }
  }

  if(line.find("</trkseg>") != std::string::npos) {
    state.pop_back();
  }
}
void GpxParser::parse_state_trkpt(std::string line, std::vector<gpx_tags>& state) {

  // std::cout << "parse state trkpt" << std::endl;
  
  auto pos = line.find("<ele>");
  if(pos != std::string::npos) {
    this->elevation = std::stod(line.substr(pos + 5, line.find("</ele>") - 6));
    
    // std::cout << "Elevation: " << std::setprecision(5) << this->elevation << std::endl;
  }
  
  pos = line.find("<time>");
  if(pos != std::string::npos) {
    std::string time_str;
    std::string tag("time");
    get_string_in_tags(line, tag, time_str);

    this->time = Helper::TimeConverter::date_time_string_to_time_t(time_str);
  }

  if(line.find("</trkpt>") != std::string::npos) {

    GpxPoint* gpx_point = new GpxPoint();
    gpx_point->lat       = this->lat;
    gpx_point->lng       = this->lng;
    gpx_point->elevation = this->elevation;
    gpx_point->time      = this->time;
    // gpx_point->print();
 
    this->data.push_back(gpx_point);

    state.pop_back();
  }
}

