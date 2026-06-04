#include <stdexcept>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"

namespace toml98 {

bool Parser::readBoolean(const Token& tok) {
  if (tok.type != TokenWord) {
    throw std::runtime_error("Is not a word in isWordBoolean.");
  }

  if (tok.value == "true") {
    return true;
  }
  if (tok.value == "false") {
    return false;
  }
  throw std::runtime_error("Invalid boolean value : " + tok.value);
}

}  // namespace toml98