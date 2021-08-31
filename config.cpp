#include "config.hpp"

#include <mongocxx/uri.hpp>

using json = nlohmann::json; 

std::string Config::get(std::string key, std::string default_value, std::string const attrs[]) {

  if(contains(key)) {
    return memoized.at(key);
  }

  std::string value; 
  try {
    value = dig(attrs);
  }
  catch (...) {
    value = default_value;
  }
  memoize(key, value);

  return value;
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

bool Config::contains(std::string key) {
  if(memoized.find(key) != memoized.end()) {
    return true;
  }
  return false;
}

void Config::memoize(std::string key, std::string value) {
  if(memoized.find(key) == memoized.end()) {
    memoized.insert(std::pair<std::string, std::string>(key, value));
  }
}

std::string Config::mongo_connect_string() {
  if(contains("mongo_connect_string")) {
    return memoized.at("mongo_connect_string");
  }

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
    memoize("mongo_connect_string", ss.str());
    return ss.str();
  }
  else  {
    return mongocxx::uri::k_default_uri;
  }
}
  
std::string Config::mongo_db_name() {
  std::string default_db_name { "test" };
  std::string attrs[] = { "mongo", "db" };
  
  return get("mongo_db_name", default_db_name, attrs);
}
  
std::string Config::gpx_folder() {
  std::string default_gpx_folder { "data/gpx" };
  std::string attrs[] = { "import", "gpx" };

  return get("gpx_folder", default_gpx_folder, attrs);
}

std::string Config::runtastic_folder() {
  std::string default_runtastic_folder { "data/Sport-sessions" };
  std::string attrs[] = { "import", "runtastic" };
  
  return get("runtastic_folder", default_runtastic_folder, attrs);
}

std::string Config::weight_folder() {
  std::string default_weight_folder { "data/Weight" };
  std::string attrs[] = { "import", "weight" };
  
  return get("weight_folder", default_weight_folder, attrs);
}

std::string Config::garmin_file() {
  std::string default_garmin_file { "data/garmin_activities.csv" };

  std::string attrs[] = { "import", "garmin" };

  return get("garmin_file", default_garmin_file, attrs);
}

std::string Config::cities_file() {
  std::string default_cities_file { "data/cities1000.txt" };
  std::string attrs[] = { "import", "cities" };

  return get("cities_file", default_cities_file, attrs);
}


std::string Config::srtm3_folder() {
  std::string default_srtm3_folder { "data/srtm3/" };

  std::string attrs[] = { "import", "srtm3" };
  return get("srtm3_folder", default_srtm3_folder, attrs);
}

uint32_t Config::pause_threshold() {
  std::string default_pause_threshold { "30" };

  std::string attrs[] = { "import", "pause_threshold" };
  std::string value = get("pause_threshold", default_pause_threshold, attrs);

  return std::stoi(value);
}
