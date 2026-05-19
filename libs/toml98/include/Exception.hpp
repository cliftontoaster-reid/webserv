
#ifndef TOML98_INCLUDE_EXCEPTION_HPP
#define TOML98_INCLUDE_EXCEPTION_HPP

#include <exception>

namespace toml98 {

class EarlyEndOfFile : public std::exception {
 public:
  const char* what() const throw() { return "Early end of file"; }
};

};  // namespace toml98

#endif