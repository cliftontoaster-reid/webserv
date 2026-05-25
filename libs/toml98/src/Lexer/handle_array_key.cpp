#include <stdexcept>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_array_key() {
  while (canPeek()) {
    char now = peek();

    switch (now) {
      case '\0':
        throw std::runtime_error("Early end of file reading array key.");

      case ' ':
      case '\t':
        _stack.push(LexerWhiteSpace);
        return NULL;

      case '.':
        pop();
        return new Token(TokenDot, ".");

      case ']':
        pop();
        if (canPeek(']')) {
          pop();
          _stack.pop();
          return new Token(TokenArrayKeyEnd, "]");
        }
        throw std::runtime_error(
            "A singular ']' cannot be in the middle of an array key.");
      case '[':
        throw std::runtime_error("Array Keys cannot contain other array keys.");

      case '"':
      case '\'': {
        char quote = now;
        pop();

        if (canPeek(quote) && canPeek(quote, 1)) {
          throw std::runtime_error(
              "Array keys cannot contain multi-line strings.");
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
            "Unterminated array key: Unknown character in LexerArrayKey");
      }
    }
  }

  throw std::runtime_error(
      "Unterminated array key: Unknown character in LexerArrayKey");
}

}  // namespace toml98