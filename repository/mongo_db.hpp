#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "../models/weight.hpp"    
#include "../models/session.hpp"
#include "../models/city.hpp"

class MongoDB {

private:
  mongocxx::instance mongodb_instance{}; // This should be done only once.
  mongocxx::client client;
  mongocxx::database db; 

  MongoDB() {
    mongocxx::uri uri("mongodb://localhost:27017");
    client = mongocxx::client(uri);
    db = client["test"];
  };

  static MongoDB* _inst;

  static bsoncxx::types::b_date time_t_to_b_date(time_t time);

public:

  static MongoDB* connection() {
    if (!_inst)
      _inst = new MongoDB ();
    return _inst;
  }

  ~MongoDB() {
    if( NULL != MongoDB::_inst ) {
      delete MongoDB::_inst;
    }
  }
  
  template <class T>
  static bsoncxx::builder::basic::array vector_to_array(std::vector<T> vec);
  
  mongocxx::collection collection(std::string name);

  static std::string new_object_id();

  void insert(Models::Weight weight);
  void insert(Models::Session session);
  void insert(Models::City& city);

  void create_geo_index();
  void create_location_index();
  
  static void sport_type_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> sport_type_ids);
  static void year_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> years);

  void list_sessions(time_t from, time_t to, std::vector<int> sport_type_ids, std::string notes);

  void build_session(bsoncxx::v_noabi::document::view data, Models::Session* session);

  bool find(std::string id, Models::Weight* weight);
  bool find(std::string id, Models::Session* session);
  bool find_nearest_city(double lat, double lng, Models::City* city, uint32_t maxdist = 10000);

  bool exists(std::string colname, std::string id);
  bool exists(time_t start_time, int sport_type_id); 
  bool city_exist(double lat, double lng);

  bool delete_one(std::string id);
  uint32_t delete_many(time_t const from, time_t const to);

  void print_collection(std::string name);
};

