#include <cctype>
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

  return new Token(TokenWord, result);
}

}  // namespace toml98
