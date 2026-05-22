#include <cstddef>
#include <stdexcept>

#include "Exception.hpp"
#include "Lexer.hpp"

namespace toml98 {

// NOLINTNEXTLINE
Token* Lexer::handle_table_key() {
  while (true) {
    char now = peek();

    switch (now) {
      case '\0':
        throw std::runtime_error("Early end of file reading table key.");

      case ' ':
      case '\t':
        _stack.push(LexerWhiteSpace);
        return NULL;

      case '.':
        pop();
        return new Token(TokenDot, ".");

      case ']':
        _stack.pop();
        pop();
        return new Token(TokenTableEnd, "]");

      case '"':
      case '\'': {
        char quote = now;
        pop();

        if (canPeek(quote) && canPeek(quote, 1)) {
          throw std::runtime_error(
              "Table keys cannot contain multi-line strings.");
        }
        if (canPeek()) {
          _stack.push(quote == '"' ? LexerStringDouble : LexerString);
        } else {
          throw std::runtime_error("Unterminated string: Early end of file.");
        }
        break;
      }

      case '-':
      case '_':  // A-Za-z0-9_- (see alphanum above)
        _stack.push(LexerWord);
        return NULL;

      default: {
        if (std::isalnum(now) != 0) {
          _stack.push(LexerWord);
          return NULL;
        }

        throw std::runtime_error(
            "Unterminated literal string: Unknown character in Normal");
      }
    }
  }
}

}  // namespace toml98