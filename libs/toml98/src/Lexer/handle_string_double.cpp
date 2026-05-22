#include <sys/types.h>

#include <stdexcept>
#include <string>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_string_double() {
  std::string result;

  while (canPeek()) {
    char now = peek();
    pop();

    switch (now) {
      case '"':
        _stack.pop();
        return new Token(TokenString, result);

      case '\\':
        handleEscapeSequence(result);
        break;

      case '\n':
      case '\r':
        throw std::runtime_error(
            "New lines are not supported unless escaped in non multi-line "
            "strings.");

      default:
        result.push_back(now);
        break;
    }
  }

  throw std::runtime_error("Unterminated string.");
}

}  // namespace toml98