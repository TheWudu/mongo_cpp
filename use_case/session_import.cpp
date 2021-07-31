#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> 

#include "session_import.hpp"


#include "../ext/json.hpp"
#include "../repository/mongo_db.hpp"
#include "../file_list.hpp"
#include "../json_parser.hpp"
#include "../gpx_parser.hpp"
#include "../helper/time_converter.hpp"
#include "../helper/sport_types.hpp"

#include "../models/session.hpp"

using json = nlohmann::json;

bool UseCase::SessionImport::session_sort (Models::Session a, Models::Session b) { 
  return (a.start_time < b.start_time); 
}

void UseCase::SessionImport::import() {
    read_runtastic_files();
    read_garmin_csv();
    read_gpx_files();
    store_to_mongo();
}

void UseCase::SessionImport::read_gpx_files() {
  FileList file_list = FileList("data/gpx");

  std::vector<std::string> files = file_list.files();

  std::cout << "Found: " << files.size() << " GPX files" << std::endl;

  for(auto filename = files.begin(); filename != files.end(); filename++) {
    GpxParser gpx = GpxParser();

    gpx.parse_file(*filename);
    Models::Session session = gpx.build_model();
    this->data.push_back(session); 
  }
}

void UseCase::SessionImport::read_garmin_csv() {
  std::string line;
  std::vector<std::string> strs;
  int cnt = 0;

  std::ifstream filestream("data/garmin_activities.csv");

  while (getline (filestream, line)) {
    boost::split(strs, line, boost::is_any_of(","));

    std::vector<std::string> strings;
    for(auto s : strs) {
      s.erase(remove(s.begin(), s.end(), '"'), s.end());
      
      strings.push_back(s);
    }
      
    if(strs[0] != "Activity Type") {
      std::string dist = strings[4];
      dist.erase(remove(dist.begin(), dist.end(), '"'), dist.end());

      Models::Session rs;
      rs.id             = MongoDB::new_object_id();
      rs.sport_type_id  = Helper::SportType::id(strings[0]); 
      rs.distance       = std::stof(dist) * 1000;
      rs.duration       = Helper::TimeConverter::time_str_to_ms(strings[6]);
      rs.elevation_gain = strings[14] == "--" ? 0 : std::stoi(strings[14]);
      rs.elevation_loss = strings[15] == "--" ? 0 : std::stoi(strings[15]);
      rs.start_time_timezone_offset = 7200;
      rs.start_time     = Helper::TimeConverter::date_time_string_to_time_t(strings[1]) - rs.start_time_timezone_offset;
      rs.end_time       = rs.start_time + rs.duration / 1000;
      rs.notes          = strings[3]; 

      cnt++;

      this->data.push_back(rs);
    }
  }
  std::cout << "Found: " << cnt << " GARMIN csv entries" << std::endl;

  filestream.close();  
}

void UseCase::SessionImport::read_runtastic_files() {
  FileList file_list = FileList("data/Sport-sessions");

  std::vector<std::string> files = file_list.files();

  std::cout << "Found: " << files.size() << " RUNTASTIC JSON files" << std::endl;

  for(auto filename = files.begin(); filename != files.end(); filename++) {
    JsonParser json_parser = JsonParser(*filename);
  
    json json_data = json_parser.get_data();

    Models::Session rs;
    rs.id          = json_data["id"];
    rs.distance    = json_data["distance"];
    rs.duration    = json_data["duration"];
    if(json_data["elevation_gain"] != nullptr) {
      rs.elevation_gain = json_data["elevation_gain"];
      rs.elevation_loss = json_data["elevation_loss"];
    }
    int64_t start_timestamp       = json_data["start_time"];
    int64_t end_timestamp         = json_data["end_time"];
    rs.start_time_timezone_offset = json_data["start_time_timezone_offset"];
    rs.start_time    = start_timestamp / 1000;
    rs.end_time      = end_timestamp / 1000;
    rs.sport_type_id = std::stoi((std::string)json_data["sport_type_id"]);
    if(json_data["notes"] != nullptr) {
      rs.notes         = json_data["notes"];
    }

    this->data.push_back(rs);
  }

  sort(this->data.begin(), this->data.end(), session_sort);
}

void UseCase::SessionImport::store_to_mongo() {
  MongoDB* mc = MongoDB::connection();
  std::string collection("sessions");
  int icnt = 0;
  int fcnt = 0;

  for(auto rs = this->data.begin(); rs != this->data.end(); rs++) {
    if (mc->exists(rs->start_time, rs->sport_type_id) == false) {
       mc->insert(*rs);
      icnt++;
    } else {
      fcnt++;
    }
  }

  std::cout << "Found: " << fcnt << ", inserted: " << icnt << std::endl;
}
