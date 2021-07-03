#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

class JsonParser
{
  json data;

  public: 

  JsonParser (std::string fn);

  void read_file(std::string filename);
  json get_data();
};

