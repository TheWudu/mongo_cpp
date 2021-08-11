#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <boost/algorithm/string.hpp>

#include "helper/args.hpp"
#include "helper/menu.hpp"
#include "helper/help.hpp"
#include "helper/time_converter.hpp"
#include "use_case/use_cases.hpp"
#include "parser/geonames_parser.hpp"

#include "repository/mongo_db.hpp"


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
      case cities_import:
        parse_args(argc, argv, args);
        import_cities(args); break;
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
