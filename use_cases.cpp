#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>

#include "mongo_db.hpp"
#include "helper/time_converter.hpp"
#include "helper/sport_types.hpp"
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
  std::vector<int> sport_type_ids;
  std::string notes;

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

  try { 
    if(args.at("-sport_type") != "all") {
      std::vector<std::string> strs;

      boost::split(strs, args.at("-sport_type"), boost::is_any_of(","));
      for(auto s : strs) {
        sport_type_ids.push_back(Helper::SportType::id(s));
      }
    }
  }
  catch (std::out_of_range&) {
    // sport_type_ids.push_back(1); // running
  }

  try {
    notes = args.at("-notes");
  }
  catch (std::out_of_range&) {
  }
  
  std::cout << "Fetching from: " << ctime(&from) 
            << "         to:   " << ctime(&to) 
            << std::endl;

  mc->list_sessions(from, to, sport_type_ids, notes);
}

void show_session(std::map<std::string, std::string> const args) {
  UseCase::SessionShow session_show;
  std::string id;

  try {
    id = args.at("-id");
  }
  catch (std::out_of_range&) {
    std::cout << "ID: ";
    std::cin >> id;
  }

  std::cout << "Finding id " << id << " ..." << std::endl;
  
  session_show.find(id);
}

std::vector<int> arg_to_int_vec(std::map<std::string, std::string> const args, std::string param) {

  std::vector<int> vec;
  std::vector<std::string> strs;

  boost::split(strs, args.at(param), boost::is_any_of(","));
  for(auto s : strs) {
    vec.push_back(std::stoi(s));
  }
  return vec;
}

std::vector<std::string> arg_to_str_vec(std::map<std::string, std::string> const args, std::string param) {
  std::vector<std::string> strs;

  boost::split(strs, args.at(param), boost::is_any_of(","));
  return strs;
}

int current_year() {
  time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  return gmtime(&now)->tm_year + 1900;
}

void show_statistics(std::map<std::string, std::string> const args) {
  MongoDB* mc = MongoDB::connection();
  std::vector<int> years;
  std::vector<int> sport_type_ids;
  std::vector<std::string> grouping;
  
  try { 
    grouping = arg_to_str_vec(args,"-group");
  }
  catch (std::out_of_range&) {
    grouping.push_back("year");
    grouping.push_back("sport_type_id");
  }
   
  try { 
    if(args.at("-year") != "all") {
      years = arg_to_int_vec(args,"-year");
    }
  }
  catch (std::out_of_range&) {
    years.push_back(current_year());
  }
  
  try { 
    if(args.at("-sport_type") != "all") {
      std::vector<std::string> strs;

      boost::split(strs, args.at("-sport_type"), boost::is_any_of(","));
      for(auto s : strs) {
        sport_type_ids.push_back(Helper::SportType::id(s));
      }
    }
  }
  catch (std::out_of_range&) {
    sport_type_ids.push_back(1); // running
  }

  mc->aggregate_stats(years, sport_type_ids, grouping);
}
