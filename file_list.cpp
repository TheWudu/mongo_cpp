#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
#include "file_list.hpp"

namespace fs = std::filesystem;

FileList::FileList (std::string dir) {
    path = dir;
}

std::vector<std::string> FileList::files() {
  std::vector<std::string> list;

  for (const auto & entry : fs::directory_iterator(path)) {
    list.push_back(entry.path());
  }

  return list;
}
