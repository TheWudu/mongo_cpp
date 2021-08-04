#pragma once

class HgtReader {

public:

  int32_t elevation(double lat, double lng);

private:

  int32_t from_lat_lng(double lat, double lng);
  int32_t from_file(std::string const filename, uint32_t position);

};
