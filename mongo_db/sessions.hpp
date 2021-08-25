#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "../models/weight.hpp"    
#include "../models/session.hpp"
#include "../models/city.hpp"

#include "base.hpp"

namespace MongoDB {
  class Sessions : public Base {
    
  private:

    mongocxx::collection collection();
    mongocxx::pool::entry client;

  public:

    Sessions() : client( MongoConnection::connection()->client() ) {
    }

    void insert(Models::Session session);

    bool find(std::string id, Models::Session* session);
    void list(time_t from, time_t to, std::vector<int> sport_type_ids, std::string notes);

    bool exists(std::string id);
    bool exists(time_t start_time, int sport_type_id); 

    bool delete_one(std::string id);
    uint32_t delete_many(time_t const from, time_t const to);
    
    void build_session(bsoncxx::v_noabi::document::view data, Models::Session* session);
  };

}
