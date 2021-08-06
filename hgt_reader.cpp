#include <iostream> 
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <climits>
#include "hgt_reader.hpp"


double HgtReader::elevation(double lat, double lng) {
  // find n,s,w,e values for which srtm3 data exists
  double lat_help = (lat / THREE_ARC_SECONDS);
  double lng_help = (lng / THREE_ARC_SECONDS);

  double north = std::ceil(lat_help) * THREE_ARC_SECONDS;
  double south = std::floor(lat_help) * THREE_ARC_SECONDS;
  double east = std::ceil(lng_help) * THREE_ARC_SECONDS;
  double west = std::floor(lng_help) * THREE_ARC_SECONDS;

  // get the elevation for these ne,nw,se,sw points
  int32_t elevation_ne = from_lat_lng(north,east);
  int32_t elevation_nw = from_lat_lng(north,west);
  int32_t elevation_se = from_lat_lng(south,east);
  int32_t elevation_sw = from_lat_lng(south,west);

  int32_t elevation_north = -INT_MAX;
  int32_t elevation_south = -INT_MAX; 
   
  // some fallbacks for edge-cases      
  if(east == west) {
    elevation_north = elevation_nw; 
    elevation_south = elevation_sw;
  }
    
  if(elevation_ne == UNKNOWN_ELEVATION) {
    elevation_north = elevation_nw;
  }
  if(elevation_nw == UNKNOWN_ELEVATION) {
    elevation_north = elevation_ne;
  }
  if(elevation_se == UNKNOWN_ELEVATION) {
    elevation_south = elevation_sw;
  }
  if(elevation_sw == UNKNOWN_ELEVATION) {
    elevation_south = elevation_se;
  }
    
  // do an interpolation if north/south are not unknown
  if(elevation_north == -INT_MAX) {
    elevation_north = (east - lng) / THREE_ARC_SECONDS * elevation_nw + (lng - west) / THREE_ARC_SECONDS * elevation_ne; // horizontal interpolation (north)
  }
    
  if(elevation_south == -INT_MAX) {
    elevation_south = (east-lng)/THREE_ARC_SECONDS*elevation_sw + (lng-west)/THREE_ARC_SECONDS*elevation_se; // horizontal interpolation (south)
  }
    
  double elevation = -0.0;
    
  if(north == south) {
    elevation = elevation_south; 
  }
    
  if(elevation_north == UNKNOWN_ELEVATION) {
    elevation = elevation_south;
  }
  if(elevation_south == UNKNOWN_ELEVATION) {
    elevation = elevation_north;
  }
    
  if(elevation == -0.0) {
    elevation = (north - lat)/THREE_ARC_SECONDS*elevation_south + (lat-south)/THREE_ARC_SECONDS*elevation_north;
  }
  
  return elevation;
}

int32_t HgtReader::from_lat_lng(double lat, double lng) {

  auto latDegreef = std::abs(lat);
  auto lngDegreef = std::abs(lng);

  auto latDegree = std::floor(latDegreef);
  auto lngDegree = std::floor(lngDegreef);


  auto latMinutef = (latDegreef - latDegree) * 60;
  auto lngMinutef = (lngDegreef - lngDegree) * 60;
        
  auto latMinute = std::floor(latMinutef);
  auto lngMinute = std::floor(lngMinutef);
  
  auto latSecond = std::round((latMinutef-latMinute)*60);
  auto lngSecond = std::round((lngMinutef-lngMinute)*60);
        
  char latOrientation = lat < 0 ? 'S' : 'N';
  char lngOrientation = lng < 0 ? 'W' : 'E';
        
  latDegree = (lat < 0) ? latDegree + 1 : latDegree;
  lngDegree = (lng < 0) ? lngDegree + 1 : lngDegree;

  char buffer[24];  
  std::sprintf(buffer, "%c%02d%c%03d.hgt",latOrientation,(uint16_t)(latDegree),lngOrientation,(uint16_t)lngDegree);
  std::string filename { buffer };
 
  // std::cout << "file " << filename << std::endl;

  auto line = latMinute * 20 + std::round(latSecond * 0.333333333);
  if(lat >= 0) {
    line = 1201 - (line + 1);
  }

  uint32_t sample = lngMinute * 20 + std::round(lngSecond * 0.333333);

  if(lng < 0) {
    sample = 1200 - sample;
  }

  // std::cout << "line " << line << " sample: " << sample << std::endl;

  auto position = (line * 1201 + sample) * 2;
  
  // std::cout << "position " << position << std::endl;

  return from_file(filename, position);
}

int32_t HgtReader::from_file(std::string const filename, uint32_t position) {
  int32_t elevation = UNKNOWN_ELEVATION;

  std::string path { "data/srtm3/" };
  std::string filepath = path + filename;
  uint8_t buffer[10] = { 0x00 };

  std::FILE* f = std::fopen(filepath.c_str(), "rb");
  if(f == nullptr) {
    // std::cout << "Missing file: " << filepath << std::endl;
    throw filename;
  }
  std::fseek(f, position, SEEK_SET);
  std::fread(buffer, 1, 2, f);
  elevation = (buffer[0] << 8 | buffer[1]);

  // If the elevation (read as an unsigned int but is an signed int actually) 
  // is lager than 32768 (which is the max value of an signed int) we have to
  // substract 65535 to become the negative value.
  if(elevation >= 32768) {
    elevation -= 65536;
  }

  std::fclose(f);

  return elevation;
}
