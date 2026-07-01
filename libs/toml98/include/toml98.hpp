#ifndef TOML98_INCLUDE_TOML98_HPP
#define TOML98_INCLUDE_TOML98_HPP

#include "Value.hpp"

namespace toml98 {

toml98::Value readTomlFile(const char* path);

}  // namespace toml98

#endif