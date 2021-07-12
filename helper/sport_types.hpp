#pragma once

#include <map>

namespace Helper {

  class SportType {

  public:

    static std::string name(int id) {
      try {
        return sport_types().at(id);
      } 
      catch(std::out_of_range const&) {
        return "unknown " + std::to_string(id);
      }
    }

    static int id(std::string name) {
      try {
        for(auto const& [k,v] : sport_types()) {
          if(v == name) {
            return k;
          }
        }
        throw std::out_of_range("Not found");
      } 
      catch(std::out_of_range const&) {
        std::cout << "Don't know id for " + name << std::endl;
        return 0;
      }
    }

  private:

    static std::map<int, std::string> sport_types() {
      static const std::map<int, std::string> sport_types_map {
        {  1, "running" },
        {  3, "cycling" },
        {  5, "other" },
        {  7, "hiking" },
        {  9, "skiing" },
        { 19, "walking" },
        { 26, "yoga" },
        { 31, "pilates" },
        { 32, "climbing" },
        { 34, "strength-training" },
        { 54, "ice-skating" },
        { 55, "sledding" },
        { 69, "crossfit" },
        { 70, "dancing" },
        { 71, "ice-hockey" },
        { 74, "gymnastics" },
        { 81, "training" }
      };
      return sport_types_map;
    }
  };
}
