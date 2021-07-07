#include <iostream>
#include <vector>
#include <string>
#include "ext/json.hpp"
#include "file_list.hpp"
#include "json_parser.hpp"
#include "mongo_db.hpp"
#include "use_case/weight_import.hpp"
#include "use_case/run_session_import.hpp"
#include "use_case/run_session_show.hpp"



int main()
{
  UseCase::WeightImport w_import;
  UseCase::RunSessionImport rs_import;
  
  w_import.import();

  rs_import.import();

  // MongoDB::connection()->print_collection("weights");
  UseCase::RunSessionShow rs_show;
  rs_show.find("f93f4a6c-dc17-457e-b833-d0613b36ff8d");
  rs_show.find("84660660-5ef0-4e37-b0ba-5973f2346fe6");
  
  return 0;
}
