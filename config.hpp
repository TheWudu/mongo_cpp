#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "ext/json.hpp"

using json = nlohmann::json; 

class Config {

public:

  static Config* instance() {
    static Config conf;
    return &conf;
  }

  std::string mongo_connect_string(); 
  std::string mongo_db_name(); 
  std::string gpx_folder();
  std::string runtastic_folder();
  std::string weight_folder();
  std::string garmin_file();
  std::string cities_file();
  std::string srtm3_folder();

private:
  
  json config_json; 
    
  Config() {
    read_config();
  }

  std::string dig(std::string const attrs[]);

  void read_config() {
    std::string filename { "config.json" };
    std::ifstream instream(filename); 

    instream >> config_json;
  }

};
