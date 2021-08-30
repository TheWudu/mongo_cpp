#include "config.hpp"

#include <mongocxx/uri.hpp>

using json = nlohmann::json; 

std::string Config::mongo_connect_string() {
  if(config_json.contains("mongo")) {
    auto mc = config_json["mongo"];

    std::stringstream ss;
    ss << "mongodb://"
       << mc["host"].get<std::string>() << ":" 
       << mc["port"] << "/";

    if(mc.contains("options")) {
      ss << "?";
      auto items = mc["options"];
      for(json::iterator item = items.begin(); item != items.end(); ) {
        ss << item.key() << "=" << item.value(); 
        if(++item != items.end()) {
          ss << "&";
        }
      }
    }
    return ss.str();
  }
  else  {
    return mongocxx::uri::k_default_uri;
  }
}
  
std::string Config::mongo_db_name() {
  std::string default_db_name { "test" };
  std::string attrs[] = { "mongo", "db" };
  try {
    return dig(attrs);
  }
  catch (...) {
    return default_db_name;
  }
}
  
std::string Config::gpx_folder() {
  std::string default_gpx_folder { "data/gpx" };

  std::string attrs[] = { "import", "gpx" };
  try {
    return dig(attrs);
  }
  catch (...) {
    return default_gpx_folder;
  }
}

std::string Config::runtastic_folder() {
  std::string default_runtastic_folder { "data/Sport-sessions" };

  std::string attrs[] = { "import", "runtastic" };
  try {
    return dig(attrs);
  }
  catch (...) {
    return default_runtastic_folder;
  }
}

std::string Config::weight_folder() {
  std::string default_weight_folder { "data/Weight" };
  std::string attrs[] = { "import", "weight" };
  try {
    return dig(attrs);
  }
  catch (...) {
    return default_weight_folder;
  }
}

std::string Config::garmin_file() {
  std::string default_garmin_file { "data/garmin_activities.csv" };

  std::string attrs[] = { "import", "garmin" };
  try {
    return dig(attrs);
  }
  catch (...) {
    return default_garmin_file;
  }
}

std::string Config::cities_file() {
  std::string default_cities_file { "data/cities1000.txt" };
  std::string attrs[] = { "import", "cities" };
  try {
    return dig(attrs);
  }
  catch (...) {
    return default_cities_file;
  }
}

std::string Config::dig(std::string const attrs[]) {
  auto c = config_json;
  for(size_t i = 0; i < attrs->length(); i++) {
    if(c.contains(attrs[i])) {
      c = c[attrs[i]];
    }
    if(i+1 == attrs->length()) {
      return c.get<std::string>();
    }
  }
  throw "NotFound";
}

std::string Config::srtm3_folder() {
  std::string default_srtm3_folder { "data/srtm3/" };

  std::string attrs[] = { "import", "srtm3" };
  try {
    return dig(attrs);
  }
  catch (...) {
    return default_srtm3_folder;
  }
}
