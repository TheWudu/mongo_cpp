#pragma once

#include <vector>
#include "../models/run_session.hpp"

namespace UseCase {
  class RunSessionImport {
  public:
    void import();

  private:

    void read_files();
    void store_to_mongo();

    std::vector<Models::RunSession> data;
  };
}
