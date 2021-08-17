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

#include "mongo_db.hpp"

mongocxx::collection MongoDB::collection(std::string name) {
  MongoConnection* mc = MongoConnection::connection();
  return mc->collection(name);
}

void MongoDB::print_collection(std::string name) {
  mongocxx::cursor cursor = collection(name).find({});
  for(auto doc : cursor) {
    std::cout << bsoncxx::to_json(doc) << "\n";
  }
}

void MongoDB::insert(Models::Weight weight) {
  auto builder = bsoncxx::builder::stream::document{};

  std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(weight.date);
  bsoncxx::types::b_date date = bsoncxx::types::b_date { tp };
  
  bsoncxx::document::value doc_value = builder
    << "id"   << weight.id
    << "type" << "weight"
    << "date" << date
    << "weight" << weight.weight
    << bsoncxx::builder::stream::finalize;

  auto coll = collection("weights");
  bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(doc_value.view());
}

void MongoDB::insert(Models::City& city) {
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

  auto coll = collection("cities");
  bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(doc_value.view());
}

void MongoDB::create_location_index() {
  auto coll = collection("cities");
  auto indexes = coll.list_indexes();
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
  
  std::cout << bsoncxx::to_json(index.view()) << std::endl;

  auto result = coll.create_index(index.view(), index_options);

  std::cout << bsoncxx::to_json(result.view()) << std::endl;
}

void MongoDB::create_geo_index() {

  auto coll = collection("cities");
  auto indexes = coll.list_indexes();
  std::string index_name { "geolocation" };

  for(auto index : indexes) {
    // std::cout << bsoncxx::to_json(index) << std::endl;
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
  
  std::cout << bsoncxx::to_json(index.view()) << std::endl;

  auto result = coll.create_index(index.view(), index_options);

  std::cout << bsoncxx::to_json(result.view()) << std::endl;
}

bool MongoDB::find_nearest_city(double lat, double lng, Models::City* city, uint32_t maxdist) {
  // > db.cities.find( { location: { $geoNear: { $geometry: { "type": "Point", coordinates: [13.15228499472141265869140625, 47.98088564537465572357177734375] } } } } ).limit(1)
  // or
  // db.cities.aggregate([ { $geoNear: { near: { "type": "Point", coordinates: [13.15228499472141265869140625, 47.98088564537465572357177734375] }, spherical: true, distanceField: "calcDistance" } }, { $limit: 1 } ] )
  
  auto coll = collection("cities");
  bsoncxx::document::value equery = document{} << bsoncxx::builder::stream::finalize; 
  if(coll.count_documents(equery.view()) == 0) {
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
  
  bsoncxx::stdx::optional<bsoncxx::document::value> result = coll.find_one(query.view(), opts);

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
  
bsoncxx::types::b_date MongoDB::time_t_to_b_date(time_t time) {
  std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(time);
  bsoncxx::types::b_date timedate = bsoncxx::types::b_date { tp };
  return timedate;
}

std::string MongoDB::new_object_id() {
  bsoncxx::oid oid{};
  return oid.to_string();
}

bool MongoDB::find(std::string id, Models::Weight* weight) {
  bsoncxx::document::value query = document{} 
    << "id"   << id
    << bsoncxx::builder::stream::finalize;

  auto coll = collection("weights");
  bsoncxx::stdx::optional<bsoncxx::document::value> result = coll.find_one(query.view());
 
  if(!result) { 
    return false;
  }
  else {
    auto data = result->view();
    std::string i = data["id"].get_utf8().value.to_string();
    float w    = data["weight"].get_double().value;
    int64_t ms = (data["date"].get_date().value).count();
    time_t t   =  ms / 1000;
  
    *weight = Models::Weight(i, t, w);
    return true;
  }
}

bool MongoDB::exists(std::string colname, std::string id) {
  bsoncxx::document::value query = document{} 
    << "id"   << id
    << bsoncxx::builder::stream::finalize;

  auto coll = collection(colname);
  int64_t count = coll.count_documents(query.view());
 
  if(count == 1) { 
    return true;
  }
  return false;
}

bool MongoDB::city_exist(double lat, double lng) {
  bsoncxx::document::value query = document{} 
    << "location"   << open_document
      << "type" << "Point" 
      << "coordinates" << open_array
        << lng << lat
        << close_array
      << close_document
    << bsoncxx::builder::stream::finalize;

  auto coll = collection("cities");
  int64_t count = coll.count_documents(query.view());

  //std::cout << bsoncxx::to_json(query.view()) << " - " << count << std::endl;
  if(count > 0) { 
    return true;
  }
  return false;
}

template <class T>
bsoncxx::builder::basic::array MongoDB::vector_to_array(std::vector<T> vec) {
  bsoncxx::builder::basic::array a = bsoncxx::builder::basic::array();
  for(auto v : vec) {
    a.append(v);
  }
  return a;
}

void MongoDB::sport_type_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> sport_type_ids) {
  using namespace bsoncxx::builder::basic;
  if(sport_type_ids.size() > 0) {
    matcher << "sport_type_id" << open_document <<
      "$in" << vector_to_array(sport_type_ids) << close_document;
  }
}
  
void MongoDB::year_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> years) {
  if(years.size() > 0) {
    matcher << "year" << open_document <<
      "$in" << vector_to_array(years) << close_document;
  }
}
 
