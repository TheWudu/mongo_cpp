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

#include "base.hpp"

mongocxx::collection MongoDB::Base::collection(std::string name) {
  MongoConnection* mc = MongoConnection::connection();
  return mc->collection(name);
}
    
mongocxx::collection MongoDB::Base::collection(mongocxx::pool::entry& c, std::string name) {
  return (*c)["test"][name];
}

void MongoDB::Base::print_collection(std::string name) {
  mongocxx::cursor cursor = collection(name).find({});
  for(auto doc : cursor) {
    std::cout << bsoncxx::to_json(doc) << "\n";
  }
}

bsoncxx::types::b_date MongoDB::Base::time_t_to_b_date(time_t time) {
  std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(time);
  bsoncxx::types::b_date timedate = bsoncxx::types::b_date { tp };
  return timedate;
}

std::string MongoDB::Base::new_object_id() {
  bsoncxx::oid oid{};
  return oid.to_string();
}

template <class T>
bsoncxx::builder::basic::array MongoDB::Base::vector_to_array(std::vector<T> vec) {
  bsoncxx::builder::basic::array a = bsoncxx::builder::basic::array();
  for(auto v : vec) {
    a.append(v);
  }
  return a;
}

void MongoDB::Base::sport_type_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> sport_type_ids) {
  using namespace bsoncxx::builder::basic;
  if(sport_type_ids.size() > 0) {
    matcher << "sport_type_id" << open_document <<
      "$in" << vector_to_array(sport_type_ids) << close_document;
  }
}
  
void MongoDB::Base::year_matcher(bsoncxx::builder::stream::document& matcher, std::vector<int> years) {
  if(years.size() > 0) {
    matcher << "year" << open_document <<
      "$in" << vector_to_array(years) << close_document;
  }
}
 
