#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility> // for std::pair

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include "../helper/time_converter.hpp"
#include "../helper/sport_types.hpp"
#include "../helper/output.hpp"

using namespace mongocxx;

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

#include "cities.hpp"

mongocxx::collection MongoDB::Cities::collection() {
  return Base::collection("cities");
}

void MongoDB::Cities::insert(Models::City& city) {
  auto builder = bsoncxx::builder::stream::document{};

  bsoncxx::document::value doc_value = builder
    << "name" << city.name
    << "timezone" << city.timezone
    << "location" << open_document
      << "type" << "Point" 
      << "coordinates" << open_array
        << city.lng << city.lat
        << close_array
      << close_document
    << bsoncxx::builder::stream::finalize;

  bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection().insert_one(doc_value.view());
}

void MongoDB::Cities::create_location_index() {
  auto indexes = collection().list_indexes();
  std::string index_name { "location" };

  for(auto index : indexes) {
    if(index["name"].get_utf8().value.to_string() == index_name) {
      std::cout << index_name << " index already exists, skip creation" << std::endl;
      return;
    }
  }

  bsoncxx::builder::stream::document index_builder = bsoncxx::builder::stream::document{};
  auto index = index_builder 
    << "location" << 1
    << bsoncxx::builder::stream::finalize;

  mongocxx::options::index index_options{};
  index_options.name(index_name);
  
  collection().create_index(index.view(), index_options);
}

void MongoDB::Cities::create_geo_index() {

  auto indexes = collection().list_indexes();
  std::string index_name { "geolocation" };

  for(auto index : indexes) {
    if(index["name"].get_utf8().value.to_string() == index_name) {
      std::cout << "Index already exists, skip creation" << std::endl;
      return;
    }
  }

  bsoncxx::builder::stream::document index_builder = bsoncxx::builder::stream::document{};
  auto index = index_builder 
    << "location" << "2dsphere" 
    << bsoncxx::builder::stream::finalize;

  mongocxx::options::index index_options{};
  index_options.name(index_name);
  
  collection().create_index(index.view(), index_options);
}

bool MongoDB::Cities::find_nearest(double lat, double lng, Models::City* city, uint32_t maxdist) {
  // > db.cities.find( { location: { $geoNear: { $geometry: { "type": "Point", coordinates: [13.15228499472141265869140625, 47.98088564537465572357177734375] } } } } ).limit(1)
  // or
  // db.cities.aggregate([ { $geoNear: { near: { "type": "Point", coordinates: [13.15228499472141265869140625, 47.98088564537465572357177734375] }, spherical: true, distanceField: "calcDistance" } }, { $limit: 1 } ] )
  
  bsoncxx::document::value equery = document{} << bsoncxx::builder::stream::finalize; 
  if(collection().count_documents(equery.view()) == 0) {
    return false;
  }
  mongocxx::options::find opts;
  opts.limit( 1 );  

  bsoncxx::document::value query = document{} 
    << "location"   << open_document 
      << "$geoNear" << open_document
        << "$geometry" << open_document
            << "type" << "Point"
            << "coordinates" << open_array
              << lng << lat
            << close_array
          << close_document 
        << "$maxDistance" << (int)maxdist
        << close_document
      << close_document
    << bsoncxx::builder::stream::finalize;
  
  bsoncxx::stdx::optional<bsoncxx::document::value> result = collection().find_one(query.view(), opts);

  //std::cout << bsoncxx::to_json(result->view()) << std::endl;
  
  if(!result) { 
    return false;
  }
  else {
    auto data = result->view();

    city->name     = data["name"].get_utf8().value.to_string();
    city->timezone = data["timezone"].get_utf8().value.to_string();
    city->lat      = data["location"]["coordinates"][1].get_double().value;
    city->lng      = data["location"]["coordinates"][0].get_double().value;
  
    return true;
  }
}

bool MongoDB::Cities::exist(double lat, double lng) {
  bsoncxx::document::value query = document{} 
    << "location"   << open_document
      << "type" << "Point" 
      << "coordinates" << open_array
        << lng << lat
        << close_array
      << close_document
    << bsoncxx::builder::stream::finalize;

  int64_t count = collection().count_documents(query.view());

  if(count > 0) { 
    return true;
  }
  return false;
}

