#ifndef MON_ROUTER_INCLUDE_PATH_HPP
#define MON_ROUTER_INCLUDE_PATH_HPP

#include <sys/stat.h>

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "Detect.hpp"

namespace mon_router {

class Path {
 private:
  std::vector<std::string> _components;
  std::string _root;  // Keep track of your base/jail directory

 public:
  explicit Path(const std::string& root) : _root(root) {}

  void append(const std::string& str) {
    std::vector<std::string> tmp;
    size_t readFrom = 0;
    size_t found = 0;

    while ((found = str.find(PATH_SEPARATOR, readFrom)) != std::string::npos) {
      if (found > readFrom) {
        tmp.push_back(str.substr(readFrom, found - readFrom));
      }
      readFrom = found + 1;
    }

    if (readFrom < str.size()) {
      tmp.push_back(str.substr(readFrom));
    }

    append(tmp);
  }
  void append(const std::vector<std::string>& components) {
    for (size_t i = 0; i < components.size(); ++i) {
      const std::string& comp = components[i];
      if (comp.empty() || comp == ".") {
        continue;
      }

      if (comp == "..") {
        if (_components.empty()) {
          throw std::runtime_error("Path: Unwinding past root");
        }
        _components.pop_back();
      } else {
        _components.push_back(comp);
      }
    }
  }

  bool resolve(std::string& resolved) const {
    resolved = _root;
    for (size_t i = 0; i < _components.size(); ++i) {
      resolved += PATH_SEPARATOR + _components[i];
    }

    struct stat file_stat;
    return stat(resolved.c_str(), &file_stat) != -1;
  }
};

}  // namespace mon_router

#endif