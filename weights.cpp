#include "weights.hpp"
#include "mongo_db.hpp"
#include "json.hpp"
#include "file_list.hpp"
#include "json_parser.hpp"

using json = nlohmann::json;

bool weight_sort (json a,json b) { 
  return (a["start_time"] < b["start_time"]); 
}
  
void Weights::import_weights() {
    read_files();
    store_to_mongo();
}

void Weights::read_files() {
  FileList file_list = FileList("data/Weight");

  std::vector<std::string> files = file_list.files();

  std::cout << "Found: " << files.size() << " files" << std::endl;

  for(auto filename = files.begin(); filename != files.end(); filename++) {
    JsonParser json_parser = JsonParser(*filename);
  
    json data = json_parser.get_data();

    this->weight_data.push_back(data);
  }

  sort(this->weight_data.begin(), this->weight_data.end(), weight_sort);
}

void Weights::store_to_mongo() {
  MongoDB* mc = MongoDB::connection();
  int icnt = 0;
  int fcnt = 0;

  for(auto data = this->weight_data.begin(); data != this->weight_data.end(); data++) {
    std::string id  = (*data)["id"];
    double weight   = (*data)["weight"];
    uint64_t timestamp = (*data)["start_time"];
    std::time_t time = timestamp / 1000;

    char buf[19];
    struct tm * timeinfo = localtime(&time);
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H-%MZ", timeinfo);
    // std::cout << "start_time: " << buf << " - " << weight << "\n";

    if (mc->weight_exists(buf) == false) {
      mc->insert_weight(id, weight, buf);
      icnt++;
    } else {
      fcnt++;
    }
  }

  std::cout << "Found: " << fcnt << ", inserted: " << icnt << std::endl;
}
