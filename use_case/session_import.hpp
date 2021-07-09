#pragma once

#include <vector>
#include "../models/session.hpp"

namespace UseCase {
  class SessionImport {
  public:
    void import();

  private:

    void read_files();
    void store_to_mongo();

    std::vector<Models::Session> data;
  };
}
