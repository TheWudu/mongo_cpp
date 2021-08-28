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
  class Weights : public Base {

  private:

    mongocxx::collection collection();
    mongocxx::pool::entry client;

  public:

    Weights() : client( MongoConnection::connection()->client()) {
    }
    
    void insert(Models::Weight weight);

    bool find(std::string id, Models::Weight* weight);

    bool exists(std::string id);
  };
}
