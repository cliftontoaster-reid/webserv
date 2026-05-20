
#ifndef TOML98_INCLUDE_EXCEPTION_HPP
#define TOML98_INCLUDE_EXCEPTION_HPP

#define STRINGIZE(x) #x
#define TODO()                                                  \
  throw std::runtime_error("TODO: not implemented at " __FILE__ \
                           ":" STRINGIZE(__LINE__))

#endif