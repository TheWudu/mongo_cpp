#include "config.hpp"

#include <mongocxx/uri.hpp>

using json = nlohmann::json; 

std::string Config::mongo_connection() {
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
    std::string s = ss.str();
    std::cout << s << std::endl;
    return ss.str();
  }
  else  {
    return mongocxx::uri::k_default_uri;
  }
}
  
std::string Config::mongo_db_name() {
  std::string default_db_name { "test" };
  if(!config_json.contains("mongo")) {
    return default_db_name;
  }
  auto mc = config_json["mongo"];
  if(!mc.contains("db")) {
    return default_db_name;
  }
  return mc["db"].get<std::string>(); 
}
