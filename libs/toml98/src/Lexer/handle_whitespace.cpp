#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_whitespace() {
  std::string result;

  while (canPeek()) {
    char now = peek();
    if (now == '\0' || (now != ' ' && now != '\t')) {
      break;
    }

    result.push_back(now);
    pop();
  }

  _stack.pop();
  if (result.empty()) {
    return NULL;
  }
  return new Token(TokenDelimiter, result);
}

}  // namespace toml98