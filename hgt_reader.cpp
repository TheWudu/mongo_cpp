#include <iostream> 
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <climits>
#include "hgt_reader.hpp"

#define UNKNOWN_ELEVATION -32768
#define THREE_ARC_SECONDS (3.0 / 60 / 60) // 0.000833333333333333333 
// 3.0 / 60 / 60)

int32_t HgtReader::elevation(double lat, double lng) {
  // find north, south values for which we have srtm3 data points
  double lat_help = (lat / THREE_ARC_SECONDS);

  double north = std::ceil(lat_help) * THREE_ARC_SECONDS;
  double south = std::floor(lat_help) * THREE_ARC_SECONDS;
      
  // find east, west values for which we have srtm3 data points
  double lng_help = (lng / THREE_ARC_SECONDS);
      
  double east = std::ceil(lng_help) * THREE_ARC_SECONDS;
  double west = std::floor(lng_help) * THREE_ARC_SECONDS;

  //now get the elevation for North East, North West, ...
  int32_t elevation_ne = from_lat_lng(north,east);
  int32_t elevation_nw = from_lat_lng(north,west);
  int32_t elevation_se = from_lat_lng(south,east);
  int32_t elevation_sw = from_lat_lng(south,west);

  int32_t elevation_north = -INT_MAX;
  int32_t elevation_south = -INT_MAX; 
         
  // if lng is aligned to center of data pixel, interpolation does not work because we took the same data point for east and west
  if(east == west) {
    elevation_north = elevation_nw; // both elevation_nw and elevation_ne would work here, because they are the same
    elevation_south = elevation_sw; // both elevation_sw and elevation_se would work here, because they are the same
  }
    
  // if one elevation is undefined we make sure that we later won't use it in the interpolation
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
    
  // normally elevation_north and elevation_south will still be nil and we will do a bilinear interpolation 
  if(elevation_north == -INT_MAX) {
    elevation_north = (east - lng) / THREE_ARC_SECONDS * elevation_nw + (lng - west) / THREE_ARC_SECONDS * elevation_ne; // horizontal interpolation (north)
  }
    
  if(elevation_south == -INT_MAX) {
    elevation_south = (east-lng)/THREE_ARC_SECONDS*elevation_sw + (lng-west)/THREE_ARC_SECONDS*elevation_se; // horizontal interpolation (south)
  }
    
  double elevation = -0.0;
    
    
  if(north == south) {
    elevation = elevation_south; // both elevation_south and elevation_north would work here, because they are the same
  }
    
  if(elevation_north == UNKNOWN_ELEVATION) {
    elevation = elevation_south;
  }
  if(elevation_south == UNKNOWN_ELEVATION) {
    elevation = elevation_north;
  }
    
  if(elevation == -0.0) {
    elevation = (north-lat)/THREE_ARC_SECONDS*elevation_south + (lat-south)/THREE_ARC_SECONDS*elevation_north;
  }
  
  return std::round(elevation);
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
  std::sprintf(buffer, "%c%02d%c%03d.hgt",latOrientation,(uint32_t)latDegree,lngOrientation,(uint32_t)lngDegree);
  std::string filename { buffer };

  auto line = latMinute * 20 + std::round(latSecond * 0.333333333);
  if(lat >= 0) {
    line = 1201 - (line + 1);
  }

  uint32_t sample = lngMinute * 20 + std::round(lngSecond * 0.333333);

  if(lng < 0) {
    sample = 1200 - sample;
  }

  auto position = (line * 1201 + sample) * 2;

  return from_file(filename, position);
}



int32_t HgtReader::from_file(std::string const filename, uint32_t position) {
  int32_t elevation = UNKNOWN_ELEVATION;

  std::string path { "data/srtm3/" };
  std::string filepath = path + filename;
  uint8_t buffer[10] = { 0x00 };

  std::FILE* f = std::fopen(filepath.c_str(), "rb");
  std::fseek(f, position, SEEK_SET);
  std::fread(buffer, 1, 2, f);
  elevation = (buffer[0] << 8 | buffer[1]);

  // If the elevation (read as an unsigned int but is an signed int actually) 
  // is lager than 32768 (which is the max value of an signed int) we have to
  // substract 65535 to become the negative value.
  if(elevation >= 32768) {
    elevation -= 65536;
  }

  return elevation;
}
