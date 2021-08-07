#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "../ext/json.hpp"

using json = nlohmann::json;

class JsonParser
{
  json data;

  public: 

  JsonParser (std::string fn);

  void read_file(std::string filename);
  json get_data();
};

