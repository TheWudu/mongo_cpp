#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
#include "json_parser.hpp"

JsonParser::JsonParser (std::string fn) {
  read_file(fn);
}

void JsonParser::read_file(std::string filename) {
  std::ifstream instream(filename); 

  instream >> data;
}

nlohmann::json JsonParser::get_data()
{
  return data;
}

