#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "../models/city.hpp"

#include "base.hpp"

namespace MongoDB {
  class Cities : public Base {
  private:

    mongocxx::collection collection();

  public:

    void insert(Models::City& city);

    void create_geo_index();
    void create_location_index();

    bool find_nearest(double lat, double lng, Models::City* city, uint32_t maxdist = 10000);

    bool exist(double lat, double lng);
  };
}
