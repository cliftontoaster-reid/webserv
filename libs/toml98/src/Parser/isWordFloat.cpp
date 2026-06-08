#include <stdexcept>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"

namespace toml98 {

bool Parser::isWordFloat(const Token& tok) {
  if (tok.type != TokenWord) {
    throw std::runtime_error("Is not a word in isWordBoolean.");
  }

  if (tok.value.find_first_not_of("0123456789") != std::string::npos) {
    return false;
  }

  if (!canPeek() || peek().type != TokenDot) {
    return false;
  }

  if (!canPeek(1) || peek(1).type != TokenWord) {
    return false;
  }
  if (peek(1).value.find_first_not_of("0123456789") != std::string::npos) {
    return false;
  }

  return true;
}

}  // namespace toml98