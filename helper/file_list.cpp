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

  try {
    for (const auto & entry : fs::directory_iterator(path)) {
      if(entry.is_regular_file()) {
        list.push_back(entry.path());
      }
    }
  }
  catch(std::filesystem::__cxx11::filesystem_error&) {
  }

  return list;
}
