#pragma once

#define UNKNOWN_ELEVATION -32768
#define THREE_ARC_SECONDS (3.0 / 60 / 60)

class HgtReader {

public:

  static double elevation(double lat, double lng);

private:

  static int32_t from_lat_lng(double lat, double lng);
  static int32_t from_file(std::string const filename, uint32_t position);

};
