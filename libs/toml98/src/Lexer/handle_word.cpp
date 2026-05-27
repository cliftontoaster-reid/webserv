#include <cctype>
#include <stdexcept>
#include <string>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_word() {
  std::string result;

  while (canPeek()) {
    char now = peek();
    if (std::isalnum(now) == 0 && now != '-' && now != '_') {
      break;
    }
    pop();
    result.push_back(now);
  }

  if (result.empty()) {
    throw std::runtime_error("Ran LexerWord without purpose.");
  }

  _stack.pop();
  return new Token(TokenWord, result);
}

}  // namespace toml98
