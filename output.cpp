#include <iomanip>
#include <iostream>
#include <algorithm>

#include "output.hpp"
#include "helper/time_converter.hpp"
#include "helper/sport_types.hpp"

void Output::print_session_list(std::vector<Models::Session> sessions) {
  using namespace std;
    
  uint32_t notes_len = 75;
  
  size_t max_size_dur = 0;
  size_t max_size_dis = 0;
  size_t max_size_typ = 0;
  for(const auto session : sessions) {
    max_size_typ = std::max(max_size_typ, Helper::SportType::name(session.sport_type_id).length()); 
    max_size_dur = std::max(max_size_dur, Helper::TimeConverter::ms_to_min_str(session.duration).length());
    max_size_dis = 6; // std::max(max_size_dis, std::to_string(session.duration / 1000.0).length());
  }

  for(auto session: sessions) {
    cout <<       setw(38) << session.id
      << " - " << setw(17) << Helper::TimeConverter::time_to_string(session.start_time)
      << " - " << setw(max_size_typ) << left << Helper::SportType::name(session.sport_type_id)
      << " - " << setw(max_size_dis) << right << std::fixed << std::setprecision(2) << (session.distance > 0 ? session.distance / 1000.0 : 0.0) 
      << " - " << setw(max_size_dur) << right << Helper::TimeConverter::ms_to_min_str(session.duration)
      << " - " << session.notes.substr(0, notes_len) << (session.notes.size() > notes_len ? "..." : "")
      << endl;
  }
  cout << endl;
}

void Output::print_vector(std::string title, std::vector<std::pair<std::string, int>> vec, std::string(*conv)(uint32_t)) {
  int min_val = vec.at(0).second;
  int max_val = vec.at(0).second;
  size_t max_size = 0;

  for(const auto& [name, value] : vec) {
    min_val = std::min(value, min_val); 
    max_val = std::max(value, max_val);
    
    max_size = std::max(max_size, conv(value).length());
  }
  int steps = max_val / 100 + 1;

  std::cout << title << ":" << std::endl << std::endl;
  
  for(const auto& [name, value] : vec) {
    int stars = value / steps + 1;

    std::cout << std::setfill(' ') 
      << std::setw(10) << name
      << std::setw(2) << "(" << std::setw(max_size) << conv(value) 
      // << "; " << std::fixed << std::setprecision(2) << (float)(value * 1.0 / min_val)   // factor from min_val
      << ") |"
      << std::setw(stars) << std::setfill('*') << "\n";
  }
  std::cout << std::endl;
}

void Output::print_track_based_stats(mongocxx::v_noabi::cursor& cursor, std::vector<std::string> grouping) {
  std::cout << std::endl << "Track based statistics:" << std::endl << std::endl;

  for(auto doc : cursor) {
    std::stringstream ss; 
    for(uint32_t i = 0; i < grouping.size(); i++) {    
      int v = doc["_id"][grouping.at(i)].get_int32().value;
      if(grouping.at(i) == "sport_type_id") {
        ss << Helper::SportType::name(v); 
      }
      else {
        ss << v;
      }
      if((i+1) < grouping.size()) { ss << "/"; }
    }

    std::string id = ss.str();
    int32_t overall_distance       = doc["overall_distance"].get_int32().value;
    int32_t overall_duration       = doc["overall_duration"].get_int32().value;
    int32_t overall_elevation_gain = doc["overall_elevation_gain"].get_int32().value;
    int32_t overall_elevation_loss = doc["overall_elevation_loss"].get_int32().value;
    int32_t overall_count          = doc["overall_count"].get_int32().value;
    double average_distance        = doc["average_distance"].get_double().value;
    double average_pace            = doc["average_pace"].get_double().value;

    std::cout << id << " (#" << overall_count << ")" << std::endl
      << "  overall_distance:       " << std::setw(10) << std::fixed << std::setprecision(1) << overall_distance / 1000.0 << " [km], overall_duration:       " << Helper::TimeConverter::ms_to_min_str(overall_duration) << std::endl
      << "  overall_elevation_gain: " << std::setw(10) << overall_elevation_gain  << " [m],  overall_elevation_loss: " << overall_elevation_loss << " [m]" << std::endl
      << "  average_distance:       " << std::setw(10) << average_distance / 1000 << " [km], average_pace:           " << Helper::TimeConverter::secs_to_min_str(average_pace) << std::endl
      << std::endl;
  }
}
