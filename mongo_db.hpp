#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "models/weight.hpp"    
#include "models/session.hpp"

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

  template <class T>
  bsoncxx::builder::basic::array vector_to_array(std::vector<T> vec);
  bsoncxx::types::b_date time_t_to_b_date(time_t time);

  static bool weekday_sort(std::pair<std::string, int>& a, std::pair<std::string, int>& b);

  void aggregate_basic_statistics(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<std::string> grouping);
  void aggregate_weekdays(std::vector<int> years, std::vector<int> sport_type_ids);
  void aggregate_hour_of_day(std::vector<int> years, std::vector<int> sport_type_ids);

  void sport_type_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> sport_type_ids);
  void year_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> years);

  std::vector<std::pair<std::string, int>> build_day_vector(mongocxx::v_noabi::cursor& cursor);
  std::vector<std::pair<std::string, int>> build_hour_vector(mongocxx::v_noabi::cursor& cursor);

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
  
  mongocxx::collection collection(std::string name);

  static std::string new_object_id();

  void insert(Models::Weight weight);
  void insert(Models::Session session);

  void list_sessions(time_t from, time_t to, std::vector<int> sport_type_ids, std::string notes);

  void build_session(bsoncxx::v_noabi::document::view data, Models::Session* session);

  bool find(std::string id, Models::Weight* weight);
  bool find(std::string id, Models::Session* session);

  bool exists(std::string colname, std::string id);
  bool exists(time_t start_time, int sport_type_id); 

  void aggregate_stats(std::vector<int> years, std::vector<int> sport_type_ids, std::vector<std::string> grouping);
  void print_collection(std::string name);
};

