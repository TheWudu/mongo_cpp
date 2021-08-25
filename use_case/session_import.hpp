#pragma once

#include <vector>
#include "../models/session.hpp"

namespace UseCase {
  class SessionImport {
  public:
    void import();

    ~SessionImport() {
      for(auto rs : data) {
        delete rs;
      }
    }

  private:
    
    void import_single();
    void import_threaded();

    // gpx import
    void read_gpx_files();
    void read_gpx_files_threaded();
    Models::Session* parse_gpx_file(std::string filename);

    // runtastic export import
    void read_runtastic_files();
    void read_runtastic_files_threaded();
    Models::Session* read_runtastic_file(std::string filename);

    // garmin csv file import
    void read_garmin_csv();

    // store data to mongo
    void store_to_mongo();
    void store_to_mongo_threaded();
    
    bool create(Models::Session* rs);

    static bool session_sort (Models::Session* a, Models::Session* b);

    std::vector<Models::Session*> data;
  };
}
