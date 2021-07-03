#include <vector>
#include "json.hpp"

class Weights {
public:
  void import_weights();

private:

  void read_files();
  void store_to_mongo();

  std::vector<nlohmann::json> weight_data;
};
