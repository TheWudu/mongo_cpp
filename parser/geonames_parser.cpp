#include <string>
#include <iostream>
#include <fstream>

#include <boost/algorithm/string.hpp>

#include "../helper/geokit.hpp"
#include "../repository/mongo_db.hpp"
#include "geonames_parser.hpp"

GeonamesParser* GeonamesParser::_inst = nullptr;

std::string GeonamesParser::timezone_for(double const lat, double const lng) {

  Models::City city;
  MongoDB mc;
  if(mc.find_nearest_city(lat, lng, &city)) {
    return city.timezone;
  }
  else {
    Models::City* c = nearest(lat, lng);
    if(c == nullptr) {
      return "UTC";
   }
    return c->timezone;
  }
}

Models::City* GeonamesParser::nearest(double const lat, double const lng) {
  Models::City* nearest = nullptr;
  uint32_t min_dist = INT_MAX;
  uint32_t dist = INT_MAX;
  if(cities.size() == 0) {
std::cout << "nearest: parse file" << std::endl;
    parse_default_file();
  }

  for(Models::City* c : cities) {
    dist = Helper::Geokit::distance_earth(lat, lng, c->lat, c->lng);
    if(dist < min_dist) {
      min_dist = dist;
      nearest = c;
    }
  }

  return nearest;
}

void GeonamesParser::parse_default_file() {
  std::string filename { "data/cities1000.txt" };
  parse_file(filename);
}

void GeonamesParser::parse_file(std::string const filename) {
  std::ifstream filestream(filename);
  std::string line;
  std::vector<std::string> strs;

  if(!filestream.is_open()) {
    std::cout << "Can't open " << filename << ", skipping" << std::endl;
    return;
  }

  std::cout << "Parsing " << filename << " ... " << std::endl;

  while(getline(filestream, line)) {
    boost::split(strs, line, boost::is_any_of("\t"));

    // int i = 0;
    // for(auto str : strs) {
    //   std::cout << "(" << i++ << ") " << str << std::endl;
    // }
    // std::cout << std::endl;

    Models::City* c = new Models::City();
    c->name = strs.at(1);
    c->lat  = std::stod(strs.at(4));
    c->lng  = std::stod(strs.at(5));
    c->timezone = strs.at(17);

    cities.push_back(c);
  }
  std::cout << "... [DONE]" << std::endl;
}

void GeonamesParser::store_to_mongo() {
  MongoDB mc;
  int icnt = 0;
  int fcnt = 0;
  if(cities.size() == 0) {
std::cout << "store to mongo parse file" << std::endl;
    parse_default_file();
  }
  
  std::cout << "Creating Indexes ... " << std::endl;
  mc.create_geo_index();
  mc.create_location_index();
  
  std::cout << " [DONE]" << std::endl;

  std::cout << "Importing " << cities.size() << " cities ..." << std::endl;

  int size = cities.size();

  Models::City cx;
  for(Models::City* c : cities) {
    // if(!mc->find_nearest_city(c->lat, c->lng, &cx, 1000)) {
    if(!mc.city_exist(c->lat, c->lng)) {
      // std::cout << std::endl << "inserting" << c->name << ", " << c->lat << ", " << c->lng << std::endl;
      mc.insert(*c);    
      icnt++;
    }
    else {
      fcnt++;
    }
    if( (icnt+fcnt) % (size/100) == 0) {
      std::cout << "." << std::flush;
    }

  }
  
  std::cout << " ... [DONE]" << std::endl;
  std::cout << "Found: " << fcnt << ", inserted: " << icnt << " cities" << std::endl;

}
