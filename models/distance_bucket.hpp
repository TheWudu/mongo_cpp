#pragma once

class DistanceBucket {
public: 
  uint32_t lower_bound;
  uint32_t upper_bound;
  uint32_t total; 
  uint32_t avg_dist;
  uint32_t sum_dist;
  uint32_t sum_dur; 
};
