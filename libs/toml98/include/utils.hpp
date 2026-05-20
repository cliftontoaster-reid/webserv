
#ifndef TOML98_INCLUDE_UTILS_HPP
#define TOML98_INCLUDE_UTILS_HPP

#include <sys/types.h>

#include <string>

namespace toml98 {

int64_t intFromHex(const std::string& s);
int64_t intFromOct(const std::string& s);
int64_t intFromBin(const std::string& s);

}  // namespace toml98

#endif