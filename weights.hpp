#pragma once

#include <vector>
#include "json.hpp"
#include "models/weight.hpp"

class Weights {
public:
  void import_weights();

private:

  void read_files();
  void store_to_mongo();

  std::vector<Weight> weight_data;
};
