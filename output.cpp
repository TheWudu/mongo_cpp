#include <iomanip>
#include <iostream>


#include "output.hpp"
#include "helper/time_converter.hpp"
#include "helper/sport_types.hpp"

void Output::print_session_list(std::vector<Models::Session> sessions) {
  using namespace std;
    
  uint32_t notes_len = 75;

  for(auto session: sessions) {
    cout <<       setw(38) << session.id
      << " - " << setw(17) << Helper::TimeConverter::time_to_string(session.start_time)
      << " - " << setw(18) << left << Helper::SportType::name(session.sport_type_id)
      << " - " << setw(5)  << right << (session.distance > 0 ? std::to_string(session.distance) : "") 
      << " - " << setw(7) << right << Helper::TimeConverter::ms_to_min_str(session.duration)
      << " - " << session.notes.substr(0, notes_len) << (session.notes.size() > notes_len ? "..." : "")
      << endl;
  }
  cout << endl;
}

void Output::print_vector(std::string title, std::vector<std::pair<std::string, int>> vec) {
  int min_val = 0;
  int max_val = 0;
    
  for(const auto& [name, value] : vec) {
    if(value > max_val) { max_val = value; }
    if(value < min_val || min_val == 0) { min_val = value; }
  }
  int range = max_val - min_val + 10;
  int steps = range / 50 + 1;

  std::cout << title << ":" << std::endl << std::endl;

  for(const auto& [name, value] : vec) {
    int stars = (value - min_val) / steps + 5;

    std::cout << std::setfill(' ') 
      << std::setw(10) << name // Helper::TimeConverter::weekday_name(i) 
      << std::setw(2) << "(" << std::setw(2) << value << ") |"
      << std::setw(stars) << std::setfill('*') << "\n";
  }
  std::cout << std::endl;
}
