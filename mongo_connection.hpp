#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
    

class MongoConnection {

private:
  mongocxx::instance mongodb_instance{}; // This should be done only once.
  mongocxx::client client;
  mongocxx::database db; 
  mongocxx::collection collection;

  MongoConnection() {
    mongocxx::uri uri("mongodb://localhost:27017");
    client = mongocxx::client(uri);
    db = client["test"];
    collection = db["my_test"];
  };

  static MongoConnection* _inst;

public:

  static MongoConnection* get_instance() {
    if (!_inst)
          _inst = new MongoConnection ();
    return _inst;
  }

  ~MongoConnection() {
    if( NULL != MongoConnection::_inst ) {
      delete MongoConnection::_inst;
    }
  }

  void insert_weight(std::string id, float weight, char* date);
  bsoncxx::stdx::optional<bsoncxx::document::value> find_weight(std::string date);
  bool weight_exists(std::string date);
  void print_collection();
};

