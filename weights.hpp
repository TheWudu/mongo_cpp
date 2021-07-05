#pragma once

#include <vector>
#include "models/weight.hpp"

class Weights {
public:
  void import_weights();

private:

  void read_files();
  void store_to_mongo();

  std::vector<Models::Weight> weight_data;
};
