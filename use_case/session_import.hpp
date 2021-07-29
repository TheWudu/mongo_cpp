#pragma once

#include <vector>
#include "../models/session.hpp"

namespace UseCase {
  class SessionImport {
  public:
    void import();

  private:

    void import_runtastic();


    void read_gpx_files();
    void read_runtastic_files();
    void read_garmin_csv();
    void store_to_mongo();

    static bool session_sort (Models::Session a, Models::Session b);

    std::vector<Models::Session> data;
  };
}
