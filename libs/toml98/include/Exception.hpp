
#ifndef TOML98_INCLUDE_EXCEPTION_HPP
#define TOML98_INCLUDE_EXCEPTION_HPP

#include <exception>

#define STRINGIZE(x) #x
#define TODO()                                                  \
  throw std::runtime_error("TODO: not implemented at " __FILE__ \
                           ":" STRINGIZE(__LINE__))

namespace toml98 {

class EarlyEndOfFile : public std::exception {
 public:
  const char* what() const throw() { return "Early end of file"; }
};

};  // namespace toml98

#endif