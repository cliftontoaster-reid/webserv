
#ifndef TOML98_INCLUDE_UTILS_HPP
#define TOML98_INCLUDE_UTILS_HPP

#include <sys/types.h>

#include <string>

enum STRT_BASE {
  HEX_BASE = 16,
  OCT_BASE = 8,
  BIN_BASE = 2,
};

namespace toml98 {

int64_t intFromHex(const std::string& str);
int64_t intFromOct(const std::string& str);
int64_t intFromBin(const std::string& str);

}  // namespace toml98

#endif