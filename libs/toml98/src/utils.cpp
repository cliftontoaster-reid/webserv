#include "utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <string>

namespace toml98 {

int64_t intFromHex(const std::string& s) {
  std::string tmp;
  if (s.substr(0, 2) == "0x" || s.substr(0, 2) == "0X") {
    tmp = s.substr(2);
  } else {
    tmp = s;
  }
  tmp.erase(std::remove(tmp.begin(), tmp.end(), '_'), tmp.end());

  return strtoll(tmp.c_str(), NULL, 16);
}

int64_t intFromOct(const std::string& s) {
  std::string tmp;
  if (s.substr(0, 2) == "0o") {
    tmp = s.substr(2);
  } else {
    tmp = s;
  }
  tmp.erase(std::remove(tmp.begin(), tmp.end(), '_'), tmp.end());

  return strtoll(s.c_str(), NULL, 8);
}

int64_t intFromBin(const std::string& s) {
  std::string tmp;
  if (s.substr(0, 2) == "0b") {
    tmp = s.substr(2);
  } else {
    tmp = s;
  }
  tmp.erase(std::remove(tmp.begin(), tmp.end(), '_'), tmp.end());

  return strtoll(s.c_str(), NULL, 2);
}

}  // namespace toml98
