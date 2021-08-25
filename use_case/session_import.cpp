#include <fstream>
#include <iostream>
#include <thread>
#include <future>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> 

#include "session_import.hpp"


#include "../ext/json.hpp"
#include "../mongo_db/sessions.hpp"
#include "../helper/file_list.hpp"
#include "../parser/json_parser.hpp"
#include "../parser/gpx_parser.hpp"
#include "../helper/time_converter.hpp"
#include "../helper/sport_types.hpp"

#include "../models/session.hpp"

using json = nlohmann::json;

bool UseCase::SessionImport::session_sort (Models::Session* a, Models::Session* b) { 
  return (a->start_time < b->start_time); 
}

void UseCase::SessionImport::import() {
  return import_threaded();
  // return import_single();
}

void UseCase::SessionImport::import_single() {
  read_runtastic_files();
  read_garmin_csv();
  read_gpx_files();
  
  sort(this->data.begin(), this->data.end(), session_sort);

  store_to_mongo();
}

void UseCase::SessionImport::import_threaded() {
  std::vector<std::thread*> threads_vec;

  std::thread thread_runtastic(&UseCase::SessionImport::read_runtastic_files_threaded, this);
  std::thread thread_garmin(&UseCase::SessionImport::read_garmin_csv, this);
  std::thread thread_gpx(&UseCase::SessionImport::read_gpx_files_threaded, this);

  threads_vec.push_back(&thread_runtastic);
  threads_vec.push_back(&thread_garmin);
  threads_vec.push_back(&thread_gpx);

  for(auto t : threads_vec) {
    (*t).join();
  }

  store_to_mongo_threaded();
}

Models::Session* UseCase::SessionImport::parse_gpx_file(std::string filename) {
  GpxParser gpx = GpxParser();

  gpx.parse_file(filename);
  Models::Session* session = gpx.build_model();

  return session;
}

void UseCase::SessionImport::read_gpx_files() {
  FileList file_list = FileList("data/gpx");

  std::vector<std::string> files = file_list.files();

  std::cout << "Found: " << files.size() << " GPX files" << std::endl;

  std::string timezone = Helper::TimeConverter::get_timezone();

  for(auto filename = files.begin(); filename != files.end(); filename++) {
    auto session = parse_gpx_file(*filename);
    this->data.push_back(session); 
  }

  Helper::TimeConverter::set_timezone(timezone);
  
  std::cout << "Read: " << files.size() << " GPX files" << std::endl;
}

void UseCase::SessionImport::read_gpx_files_threaded() {
  FileList file_list = FileList("data/gpx");

  std::vector<std::string> files = file_list.files();

  std::cout << "Found: " << files.size() << " GPX files" << std::endl;

  std::string timezone = Helper::TimeConverter::get_timezone();

  std::vector<std::future<Models::Session*>> futures;

  for(auto filename = files.begin(); filename != files.end(); filename++) {
    futures.push_back(std::async(&UseCase::SessionImport::parse_gpx_file, this, *filename));
  }
  for(auto &result : futures) {
    auto session = result.get();
    this->data.push_back(session); 
  }

  Helper::TimeConverter::set_timezone(timezone);
  
  std::cout << "Read: " << files.size() << " GPX files" << std::endl;
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

      Models::Session* rs = new Models::Session;
      rs->id             = MongoDB::Base::new_object_id();
      rs->sport_type_id  = Helper::SportType::id(strings[0]); 
      rs->distance       = std::stof(dist) * 1000;
      rs->duration       = Helper::TimeConverter::time_str_to_ms(strings[6]);
      rs->pause          = 0;
      rs->elevation_gain = strings[14] == "--" ? 0 : std::stoi(strings[14]);
      rs->elevation_loss = strings[15] == "--" ? 0 : std::stoi(strings[15]);
      rs->start_time     = Helper::TimeConverter::date_time_string_to_time_t(strings[1]);
      rs->start_time_timezone_offset = Helper::TimeConverter::local_timezone_offset(rs->start_time);
      rs->start_time    -= rs->start_time_timezone_offset;
      rs->end_time       = rs->start_time + rs->duration / 1000;
      rs->notes          = strings[3]; 

      cnt++;

      this->data.push_back(rs);
    }
  }
  std::cout << "Found: " << cnt << " GARMIN csv entries" << std::endl;

  filestream.close();  
}

Models::Session* UseCase::SessionImport::read_runtastic_file(std::string filename) {
  JsonParser json_parser = JsonParser(filename);
  
  json json_data = json_parser.get_data();

  Models::Session* rs = new Models::Session;
  rs->id          = json_data["id"];
  rs->distance    = json_data["distance"];
  rs->duration    = json_data["duration"];
  if(json_data["pause_duration"] != nullptr) {
    rs->pause       = json_data["pause_duration"];
  }
  if(json_data["elevation_gain"] != nullptr) {
    rs->elevation_gain = json_data["elevation_gain"];
    rs->elevation_loss = json_data["elevation_loss"];
  }
  int64_t start_timestamp       = json_data["start_time"];
  int64_t end_timestamp         = json_data["end_time"];
  rs->start_time_timezone_offset = json_data["start_time_timezone_offset"];
  rs->start_time    = start_timestamp / 1000;
  rs->end_time      = end_timestamp / 1000;
  rs->sport_type_id = std::stoi((std::string)json_data["sport_type_id"]);
  if(json_data["notes"] != nullptr) {
    rs->notes         = json_data["notes"];
  }

  return rs;
}

void UseCase::SessionImport::read_runtastic_files() {
  FileList file_list = FileList("data/Sport-sessions");

  std::vector<std::string> files = file_list.files();

  std::cout << "Found: " << files.size() << " RUNTASTIC JSON files" << std::endl;

  for(auto filename = files.begin(); filename != files.end(); filename++) {
    Models::Session* rs = read_runtastic_file(*filename);

    this->data.push_back(rs);
  }

  std::cout << "Read: " << files.size() << " RUNTASTIC JSON files" << std::endl;
}

void UseCase::SessionImport::read_runtastic_files_threaded() {
  FileList file_list = FileList("data/Sport-sessions");

  std::vector<std::string> files = file_list.files();

  std::cout << "Found: " << files.size() << " RUNTASTIC JSON files" << std::endl;

  std::vector<std::future<Models::Session*>> futures;

  for(auto filename = files.begin(); filename != files.end(); filename++) {
    futures.push_back(std::async(&UseCase::SessionImport::read_runtastic_file, this, *filename));
  }

  for(auto & result : futures) {
    Models::Session* rs = result.get();

    this->data.push_back(rs);
  }

  std::cout << "Read: " << files.size() << " RUNTASTIC JSON files" << std::endl;
}

bool UseCase::SessionImport::create(Models::Session* rs) {
  MongoDB::Sessions sessions;
 
  if (sessions.exists(rs->start_time, rs->sport_type_id) == false) {
    sessions.insert(*rs);
    return true;
  }
  return false;
}

void UseCase::SessionImport::store_to_mongo() {
  int icnt = 0;
  int fcnt = 0;

  std::cout << "Storing to mongo_db ..." << std::endl << std::flush;

  for(auto rs : data) {
    if(create(rs)) {
      icnt++;
    } else {
      fcnt++;
    }
  }

  std::cout << "Found: " << fcnt << ", inserted: " << icnt << std::endl;
}

void UseCase::SessionImport::store_to_mongo_threaded() {
  int icnt = 0;
  int fcnt = 0;

  std::cout << "Storing to mongo_db ..." << std::endl << std::flush;

  std::vector<std::future<bool>> futures;

  for(auto rs : data) {
    futures.push_back(std::async(&UseCase::SessionImport::create, this, rs));
  }
    
  for(auto &result : futures) {
    if(result.get()) {
      icnt++;
    } else {
      fcnt++;
    }
  }

  std::cout << "Found: " << fcnt << ", inserted: " << icnt << std::endl;
}
