#include "session_import.hpp"

#include "../ext/json.hpp"
#include "../mongo_db.hpp"
#include "../file_list.hpp"
#include "../json_parser.hpp"

#include "../models/session.hpp"

using json = nlohmann::json;

bool session_sort (Models::Session a, Models::Session b) { 
  return (a.start_time < b.start_time); 
}

void UseCase::SessionImport::import() {
    read_files();
    store_to_mongo();
}

void UseCase::SessionImport::read_files() {
  FileList file_list = FileList("data/Sport-sessions");

  std::vector<std::string> files = file_list.files();

  std::cout << "Found: " << files.size() << " files" << std::endl;

  for(auto filename = files.begin(); filename != files.end(); filename++) {
    JsonParser json_parser = JsonParser(*filename);
  
    json json_data = json_parser.get_data();

    Models::Session rs;
    rs.id          = json_data["id"];
    rs.distance    = json_data["distance"];
    rs.duration    = json_data["duration"];
    int64_t start_timestamp       = json_data["start_time"];
    int64_t end_timestamp         = json_data["end_time"];
    rs.start_time_timezone_offset = json_data["start_time_timezone_offset"];
    rs.start_time    = start_timestamp / 1000;
    rs.end_time      = end_timestamp / 1000;
    rs.sport_type_id = std::stoi((std::string)json_data["sport_type_id"]);
    if(json_data["notes"] != nullptr) {
      rs.notes         = json_data["notes"];
    }

    // rs.print();

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
    if (mc->exists(collection, rs->id) == false) {
      mc->insert(*rs);
      icnt++;
    } else {
      fcnt++;
    }
  }

  std::cout << "Found: " << fcnt << ", inserted: " << icnt << std::endl;
}
