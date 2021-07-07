#include "weight_import.hpp"

#include "../ext/json.hpp"
#include "../mongo_db.hpp"
#include "../file_list.hpp"
#include "../json_parser.hpp"

#include "../models/weight.hpp"

using json = nlohmann::json;

bool weight_sort (Models::Weight a, Models::Weight b) { 
  return (a.date < b.date); 
}

std::string time_str(time_t time) {
  char buf[19];
  struct tm * timeinfo = localtime(&time);
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H-%MZ", timeinfo);

  return std::string { buf };
}
  
void UseCase::WeightImport::import() {
    read_files();
    store_to_mongo();
}

void UseCase::WeightImport::read_files() {
  FileList file_list = FileList("data/Weight");

  std::vector<std::string> files = file_list.files();

  std::cout << "Found: " << files.size() << " files" << std::endl;

  for(auto filename = files.begin(); filename != files.end(); filename++) {
    JsonParser json_parser = JsonParser(*filename);
  
    json data = json_parser.get_data();
    std::string id  = data["id"];
    double weight   = data["weight"];
    int64_t timestamp = data["start_time"];
    std::time_t time = timestamp / 1000;

    Models::Weight w = Models::Weight(id, time, weight);
    // w.print();

    this->weight_data.push_back(w);
  }

  sort(this->weight_data.begin(), this->weight_data.end(), weight_sort);
}

void UseCase::WeightImport::store_to_mongo() {
  MongoDB* mc = MongoDB::connection();
  int icnt = 0;
  int fcnt = 0;

  for(auto weight = this->weight_data.begin(); weight != this->weight_data.end(); weight++) {
    if (mc->weight_exists(weight->id) == false) {
      mc->insert_weight(*weight);
      icnt++;
    } else {
      fcnt++;
    }
  }

  std::cout << "Found: " << fcnt << ", inserted: " << icnt << std::endl;
}
