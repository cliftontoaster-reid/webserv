#include <stdexcept>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"

namespace toml98 {

bool Parser::isWordBoolean(const Token& tok) {
  if (tok.type != TokenWord) {
    throw std::runtime_error("Is not a word in isWordBoolean.");
  }

  return tok.value == "true" || tok.value == "false";
}

}  // namespace toml98