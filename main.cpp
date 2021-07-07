#include <iostream>
#include <vector>
#include <string>
#include "ext/json.hpp"
#include "file_list.hpp"
#include "json_parser.hpp"
#include "mongo_db.hpp"
#include "use_case/weight_import.hpp"
#include "use_case/run_session_import.hpp"



int main()
{
  UseCase::WeightImport w_import;
  UseCase::RunSessionImport rs_import;
  
  w_import.import();

  rs_import.import();

  // MongoDB::connection()->print_collection("weights");
  
  return 0;
}
