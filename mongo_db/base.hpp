#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "../models/city.hpp"

#include "mongo_connection.hpp"

namespace MongoDB {
  class Base {

  public:
    static bsoncxx::types::b_date time_t_to_b_date(time_t time);
    
    template <class T>
    static bsoncxx::builder::basic::array vector_to_array(std::vector<T> vec);
    
    mongocxx::collection collection(mongocxx::pool::entry& c, std::string name);

    static std::string new_object_id();

    void insert(Models::City& city);

    void create_geo_index();
    void create_location_index();
    
    static void sport_type_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> sport_type_ids);
    static void year_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> years);

    bool find_nearest_city(double lat, double lng, Models::City* city, uint32_t maxdist = 10000);

    bool city_exist(double lat, double lng);

    void print_collection(std::string name);
  };
}
