#pragma once

class Weight {

public:

  std::string id;
  time_t      date;
  float       weight; 

  Weight(std::string id, time_t date, float weight);

  void print();
  std::string time_str(time_t time);
};
