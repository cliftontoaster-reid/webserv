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
  _stack.pop();
  return NULL;
}

}  // namespace toml98