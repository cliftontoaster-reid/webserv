#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_comments() {
  while (canPeek()) {
    char now = peek();
    if (now == '\n' || now == '\0') {
      break;
    }

    pop();
  }
  return new Token(TokenNewLine, "\n");
}

}  // namespace toml98