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

void list_sessions() {
  MongoDB* mc = MongoDB::connection();
  std::string from_str;
  std::string to_str;
  time_t from; 
  time_t to;


  std::cout << "  From (YYYY-MM-DD): ";
  std::getline(std::cin, from_str);
  std::cout << "  To   (YYYY-MM-DD): ";
  std::getline(std::cin, to_str);

  if(from_str.empty()) {
    time_t SECONDS_IN_DAY = 60 * 60 * 24;
    from = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    from -= (30 * SECONDS_IN_DAY);
  }
  else {
    from = Helper::TimeConverter::string_to_time_t(from_str);
  }
  if(to_str.empty()) {
    to = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  }
  else {
    to = Helper::TimeConverter::string_to_time_t(to_str);
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
