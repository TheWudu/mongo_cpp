#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
    

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

  void insert_weight(std::string id, float weight, char* date);
  bsoncxx::stdx::optional<bsoncxx::document::value> find_weight(std::string date);
  bool weight_exists(std::string date);
  void print_collection(std::string name);
};

