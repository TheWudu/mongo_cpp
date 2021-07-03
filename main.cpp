#include <iostream>
#include <vector>
#include <string>
#include "json.hpp"
#include "file_list.hpp"
#include "json_parser.hpp"
#include "mongo_connection.hpp"

using json = nlohmann::json;

bool weight_sort (json a,json b) { 
  return (a["start_time"] < b["start_time"]); 
}

int main()
{
  FileList file_list = FileList("data/Weight");

  std::vector<std::string> files = file_list.files();
  std::vector<json> weight_data;

  for(auto filename = files.begin(); filename != files.end(); filename++) {
    JsonParser json_parser = JsonParser(*filename);
  
    json data = json_parser.get_data();

    weight_data.push_back(data);
  }

  sort(weight_data.begin(), weight_data.end(), weight_sort);

  MongoConnection* mc = MongoConnection::get_instance();

  for(auto data = weight_data.begin(); data != weight_data.end(); data++) {
    std::string id  = (*data)["id"];
    double weight   = (*data)["weight"];
    uint64_t timestamp = (*data)["start_time"];
    std::time_t time = timestamp / 1000;

    char buf[19];
    struct tm * timeinfo = localtime(&time);
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H-%MZ", timeinfo);
    std::cout << "start_time: " << buf << " - " << weight << "\n";

    if (mc->weight_exists(buf) == false) {
      mc->insert_weight(id, weight, buf);
    }
  }

  mc->print_collection();

  return 0;
}
