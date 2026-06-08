#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"

namespace toml98 {

int64_t Parser::readInteger(const Token& tok) {
  if (tok.type != TokenWord) {
    throw std::runtime_error("Is not a word in isWordBoolean.");
  }

  if (!isWordDigit(tok)) {
    throw std::runtime_error("Invalid number : " + tok.value);
  }

  return std::strtol(tok.value.c_str(), NULL, 10);
}

}  // namespace toml98