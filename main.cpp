#include <iostream>
#include <vector>
#include <string>
#include "ext/json.hpp"
#include "file_list.hpp"
#include "json_parser.hpp"
#include "mongo_db.hpp"
#include "use_case/weight_import.hpp"
#include "use_case/session_import.hpp"
#include "use_case/session_show.hpp"


int main_menu() {
  std::string option;

  std::cout << "Menu" << std::endl << std::endl;
  std::cout << "  1) Import Weights" << std::endl;
  std::cout << "  2) Import Sessions" << std::endl;
  std::cout << "  3) List Sessions" << std::endl;
  std::cout << "  4) Show Session" << std::endl;
  std::cout << "  5) Show Statistics" << std::endl;
  std::cout << std::endl;
  std::cout << "  0) EXIT" << std::endl;
  std::cout << " > ";

  std::cin >> option;

  return std::stoi(option);
}

void import_weights() {
  UseCase::WeightImport w_import;
  w_import.import();
}

void import_sessions() {
  UseCase::SessionImport rs_import;
  rs_import.import();
}

void list_sessions() {
}

void show_session() {
  UseCase::SessionShow session_show;
}

void show_statistics() {
  MongoDB* mc = MongoDB::connection();
  mc->aggregate_stats();
}

int main()
{

  int option;

  do {
    option = main_menu();

    switch(option) {
      case 1: 
        import_weights();
        break;
      case 2: 
        import_sessions();
        break;
      case 3: 
        list_sessions();
        break;
      case 4: 
        show_session(); 
        break;
      case 5: 
        show_statistics();
        break;
      case 0: 
        std::cout << "Exiting..." << std::endl;
        break;
      default: 
        std::cout << "Unknown option" << std::endl;
    }

  } while(option != 0);
  return 0;
}
