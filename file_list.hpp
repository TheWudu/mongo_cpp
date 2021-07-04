#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

class FileList
{
  std::string path;

  public:

  FileList(std::string dir); 
  std::vector<std::string> files();
};

