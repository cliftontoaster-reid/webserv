#include "utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <string>

namespace toml98 {

int64_t intFromHex(const std::string& str) {
  std::string tmp;
  if (str.substr(0, 2) == "0x" || str.substr(0, 2) == "0X") {
    tmp = str.substr(2);
  } else {
    tmp = str;
  }
  tmp.erase(std::remove(tmp.begin(), tmp.end(), '_'), tmp.end());

  return strtoll(tmp.c_str(), NULL, HEX_BASE);
}

int64_t intFromOct(const std::string& str) {
  std::string tmp;
  if (str.substr(0, 2) == "0o") {
    tmp = str.substr(2);
  } else {
    tmp = str;
  }
  tmp.erase(std::remove(tmp.begin(), tmp.end(), '_'), tmp.end());

  return strtoll(str.c_str(), NULL, OCT_BASE);
}

int64_t intFromBin(const std::string& str) {
  std::string tmp;
  if (str.substr(0, 2) == "0b") {
    tmp = str.substr(2);
  } else {
    tmp = str;
  }
  tmp.erase(std::remove(tmp.begin(), tmp.end(), '_'), tmp.end());

  return strtoll(str.c_str(), NULL, BIN_BASE);
}

}  // namespace toml98
