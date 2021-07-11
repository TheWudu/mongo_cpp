#include <iostream>
#include <string>
#include "mongo_db.hpp"
#include "helper/time_converter.hpp"
#include "use_case/weight_import.hpp"
#include "use_case/session_import.hpp"
#include "use_case/session_show.hpp"

#include "use_cases.hpp"

void import_weights() {
  UseCase::WeightImport w_import;
  w_import.import();
}

void import_sessions() {
  UseCase::SessionImport rs_import;
  rs_import.import();
}

void list_sessions(std::map<std::string, std::string> const args) {
  MongoDB* mc = MongoDB::connection();
  time_t from; 
  time_t to;

  try { 
    from = Helper::TimeConverter::string_to_time_t(args.at("-from"));
  }
  catch (std::out_of_range&) {
    time_t SECONDS_IN_DAY = 60 * 60 * 24;
    from = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    from -= (30 * SECONDS_IN_DAY);
  }

  try { 
    to = Helper::TimeConverter::string_to_time_t(args.at("-to"));
  }
  catch (std::out_of_range&) {
    to = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  }
  
  std::cout << "Fetching from: " << ctime(&from) << " to: " << ctime(&to) << std::endl;

  mc->list_sessions(from, to);
}

void show_session() {
  UseCase::SessionShow session_show;
  std::string id;

  std::cout << "ID: ";
  std::cin >> id;

  std::cout << "Finding id " << id << " ..." << std::endl;
  
  session_show.find(id);
}

void show_statistics() {
  MongoDB* mc = MongoDB::connection();
  mc->aggregate_stats();
}
