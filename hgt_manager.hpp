#pragma once

class HgtManager {

public:

  int32_t elevation(double lat, double lng);

private:

  int32_t from_file(std::string const filename, uint32_t position);

};
