#pragma once

#include <vector>
#include "../models/weight.hpp"

namespace UseCase {
  class WeightImport {
  public:
    void import();

  private:

    void read_files();
    void store_to_mongo();

    std::vector<Models::Weight> weight_data;
  };
}
