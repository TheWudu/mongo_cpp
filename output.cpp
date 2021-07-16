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
