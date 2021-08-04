#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <boost/algorithm/string.hpp>

#include "helper/args.hpp"
#include "helper/time_converter.hpp"
#include "helper/menu.hpp"
#include "helper/help.hpp"
#include "hgt_reader.hpp"
#include "use_cases.hpp"


void parse_args(int argc, char* argv[], std::map<std::string, std::string>& args) {
  std::vector<std::string> strs;

  for(int i = 2; i < argc; i++) {
    boost::split(strs, argv[i], boost::is_any_of("="));

    args.insert({strs[0], strs[1]});
  }

  for(const auto& [name, value] : args) {
    std::cout << "arg: " << name << " - " << value << std::endl;
  }
}

int main(int argc, char* argv[])
{
  HgtReader hgt;
  // should result in ~ 597
  double lat = 47.98080953769385814666748046875; 
  double lng = 13.1520300172269344329833984375;

  int32_t ele = hgt.elevation(lat,lng);
  std::cout << "Elevation: " << ele << std::endl;
  return 0;


  if(argc > 1) {

    Arguments command = argv_to_arg(argv[1]);
    std::map<std::string, std::string> args;

    switch(command) {
      case menu: 
        menu_mode(); break;
      case ls: 
        parse_args(argc, argv, args);
        list_sessions(args); break;
      case show: 
        parse_args(argc, argv, args);
        show_session(args); break;
      case del: 
        parse_args(argc, argv, args);
        delete_sessions(args); break;
      case stats:
        parse_args(argc, argv, args);
        show_statistics(args); break;
      case weight_import:
        import_weights(); break;
      case session_import:
        import_sessions(); break;
      case help:
        if(argc == 3) {
          show_help(argv[2]);
        }
        else {
          show_help(); 
        }
        break;
      case unknown:
      default:
        std::cout << "Unknown arg: '" << argv[1] << "'" << std::endl;
    }
  }
  else {
    show_help();
  }

  return 0;
}
