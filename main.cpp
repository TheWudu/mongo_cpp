#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <boost/algorithm/string.hpp>

#include "helper/menu.hpp"
#include "use_cases.hpp"


enum Arguments {
  menu,
  ls,
  session_import,
  stats,
  show,
  weight_import,
  // unknown input
  unknown
};

Arguments argv_to_arg(std::string const arg) {
  if (arg == "menu")           { return menu; }
  if (arg == "ls")             { return ls; }
  if (arg == "session_import") { return session_import; }
  if (arg == "weight_import")  { return weight_import;  }
  if (arg == "show")           { return show; }
  if (arg == "stats")          { return stats; }
  return unknown;
}

int main(int argc, char* argv[])
{
  if(argc > 1) {

    std::vector<std::string> strs;
    std::map<std::string, std::string> args;

    for(int i = 2; i < argc; i++) {
      boost::split(strs, argv[i], boost::is_any_of("="));

      args.insert({strs[0], strs[1]});
    }

    for(const auto& [name, value] : args) {
      std::cout << "arg: " << name << " - " << value << std::endl;
    }

    switch(argv_to_arg(argv[1])) {
      case menu: 
        menu_mode(); break;
      case ls: 
        list_sessions(args); break;
      case show: 
        show_session(); break;
      case stats:
        show_statistics(); break;
      case weight_import:
        import_weights(); break;
      case session_import:
        import_sessions(); break;
      case unknown:
      default:
        std::cout << "Unknown arg: '" << argv[1] << "'" << std::endl;
    }
  }

  return 0;
}
