#include <iostream>
#include <string>
#include <limits>

#include "menu.hpp"
#include "../use_cases.hpp"

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
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

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
}
