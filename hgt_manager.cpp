#include <iostream> 
#include <cstdio>
#include "hgt_manager.hpp"

#define UNKNOWN_ELEVATION -32768

int32_t HgtManager::elevation(double lat, double lng) {

  auto latDegreef = std::abs(lat);
  auto lngDegreef = std::abs(lng);

  auto latDegree = (uint32_t)latDegreef;
  auto lngDegree = (uint32_t)lngDegreef;


  auto latMinutef = (latDegreef - latDegree) * 60;
  auto lngMinutef = (lngDegreef - lngDegree) * 60;
        
  auto latMinute = (uint32_t)latMinutef;
  auto lngMinute = (uint32_t)lngMinutef;
  
  auto latSecond = (uint32_t)((latMinutef-latMinute)*60);
  auto lngSecond = (uint32_t)((lngMinutef-lngMinute)*60);
        
  char latOrientation = lat < 0 ? 'S' : 'N';
  char lngOrientation = lng < 0 ? 'W' : 'E';
        
  latDegree = (lat < 0) ? latDegree + 1 : latDegree;
  lngDegree = (lng < 0) ? lngDegree + 1 : lngDegree;
       
  char buffer[24];  
  std::sprintf(buffer, "%c%02d%c%03d.hgt",latOrientation,latDegree,lngOrientation,lngDegree);
  std::string filename { buffer };

  std::cout << "File: " << filename << std::endl;

  auto line = latMinute * 20 + (uint32_t)(latSecond * 0.333333333);
  if(lat >= 0) {
    line = 1201 - (line + 1);
  }

  uint32_t sample = lngMinute * 20 + (uint32_t)(lngSecond * 0.333333);

  if(lng < 0) {
    sample = 1200 - sample;
  }

  auto position = (line * 1201 + sample) * 2;

  return from_file(filename, position);
}



int32_t HgtManager::from_file(std::string const filename, uint32_t position) {
  int32_t elevation = UNKNOWN_ELEVATION;

  std::string path { "data/srtm3/" };
  std::string filepath = path + filename;
  char buffer[10];

  std::cout << "Read : "<< filepath << std::endl;

  std::cout << "fopen " << std::endl;
  std::FILE* f = std::fopen(filepath.c_str(), "rb");
  std::cout << "fseek " << position << std::endl;
  std::fseek(f, position, SEEK_SET);
  std::cout << "fread" << std::endl;
  std::fread(buffer, 1, 2, f);
  // elevation = f.read(2).unpack('n').first
  elevation = (buffer[0] << 8 | buffer[0]);

  std::cout << "Buffer: (" << buffer[0] << buffer[1] << ")" << elevation << std::endl;

  // If the elevation (read as an unsigned int but is an signed int actually) 
  // is lager than 32768 (which is the max value of an signed int) we have to
  // substract 65535 to become the negative value.
  if(elevation >= 32768) {
    elevation -= 65536;
  }

  return elevation;
}
