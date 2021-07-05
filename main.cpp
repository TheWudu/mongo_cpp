#include <iostream>
#include <vector>
#include <string>
#include "ext/json.hpp"
#include "file_list.hpp"
#include "json_parser.hpp"
#include "mongo_db.hpp"
#include "weights.hpp"



int main()
{
  Weights weights;
  
  weights.import_weights();

  // MongoDB::connection()->print_collection("weights");

  return 0;
}
