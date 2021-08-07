#include <iostream>
#include <string>
#include <limits>
#include <map>

#include "menu.hpp"
#include "../use_case/use_cases.hpp"

int show_main_menu() {
  std::string option;

  std::cout << std::endl << std::endl;
  std::cout << std::endl << std::endl;
  std::cout << "Menu" << std::endl << std::endl;
  std::cout << "  1) Import Weights" << std::endl;
  std::cout << "  2) Import Sessions" << std::endl;
  std::cout << "  3) List Sessions" << std::endl;
  std::cout << "  4) Show Session" << std::endl;
  std::cout << "  5) Show Statistics" << std::endl;
  std::cout << std::endl;
  std::cout << "  0) EXIT" << std::endl << std::endl;
  std::cout << " > ";

  std::cin >> option;
  std::cout << std::endl;

  return std::stoi(option);
}

void menu_mode() {
  int option;

  do {
    option = show_main_menu();

    std::map<std::string, std::string> args;
    std::string year_str;
    std::string from_str;
    std::string to_str;
    std::string sport_type_str;
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

    switch(option) {
      case 1: 
        import_weights();
        break;
      case 2: 
        import_sessions();
        break;
      case 3: 

        std::cout << "  From (YYYY-MM-DD): ";
        std::getline(std::cin, from_str);
        std::cout << "  To   (YYYY-MM-DD): ";
        std::getline(std::cin, to_str);
        std::cout << "  SportType:         ";
        std::getline(std::cin, sport_type_str);

        if(!from_str.empty()) {
          args.insert({"-from", from_str});
        }
        if(!to_str.empty()) {
          args.insert({"-to", to_str});
        }
        if(!sport_type_str.empty()) {
          args.insert({"-sport_type", sport_type_str});
        }

        list_sessions(args);
        break;
      case 4: 
        show_session(args); 
        break;
      case 5: 
        std::cout << "  Year (YYYY):";
        std::getline(std::cin, year_str);
        if(!year_str.empty()) {
          args.insert({"-year", year_str});
        }
        show_statistics(args);
        break;
      case 0: 
        std::cout << "Exiting..." << std::endl;
        break;
      default: 
        std::cout << "Unknown option" << std::endl;
    }

  } while(option != 0);
}
