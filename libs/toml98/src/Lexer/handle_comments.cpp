#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_comments() {
  while (peek() != '\n' && peek() != '\0') {
    pop();
  }
  return new Token(TokenNewLine, "\n");
}

}  // namespace toml98