#include <stdexcept>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"

namespace toml98 {

bool Parser::isWordDigit(const Token& tok) {
  if (tok.type != TokenWord) {
    throw std::runtime_error("Is not a word in isWordBoolean.");
  }

  return !tok.value.empty() &&
         tok.value.find_first_not_of("0123456789") == std::string::npos;
}

}  // namespace toml98