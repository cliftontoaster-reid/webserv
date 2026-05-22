#include <sys/types.h>

#include <stdexcept>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_normal() {
  char now = peek();

  // A-Za-z0-9_- (see the last two checks bellow)
  if (std::isalnum(now) != 0) {
    _stack.push(LexerWord);
    return NULL;
  }
  switch (now) {
    case '#':
      pop();
      _stack.push(LexerComments);
      break;

    case '{':
      pop();
      _stack.push(LexerInlineTable);
      break;

    case ' ':
    case '\t':
      _stack.push(LexerWhiteSpace);
      break;

    case '"':
    case '\'': {
      char quote = now;
      pop();

      if (canPeek(quote) && canPeek(quote, 1)) {
        pop(2);
        _stack.push(quote == '"' ? LexerStringDoubleMultiLine
                                 : LexerStringMultiLine);
      } else if (canPeek()) {
        _stack.push(quote == '"' ? LexerStringDouble : LexerString);
      } else {
        throw std::runtime_error("Unterminated string: Early end of file.");
      }
      break;
    }

    case '[': {
      pop();
      if (peek() != '[') {
        _stack.push(LexerTableKey);
        return new Token(TokenTableStart, "[");
      }
      pop();
      if (_isLastEqual) {
        _stack.push(LexerInlineArray);
      } else {
        _stack.push(LexerTableKey);
      }
    }

    case '-':
    case '_':  // A-Za-z0-9_- (see alphanum above)
      _stack.push(LexerWord);
      return NULL;

    case '.':
      pop();
      return new Token(TokenDot, ".");

    case '\n':
      pop();
      return new Token(TokenNewLine, "\n");

    case '\r':
      if (canPeek('\n')) {
        pop(2);
        return new Token(TokenNewLine, "\r\n");
      }
      throw std::runtime_error(
          "TOML does not support Carriage Return new lines, "
          "change it to '\\n' (Linux) or '\\r\\n' (Windows)");

    default:
      throw std::runtime_error("Unknown character in Normal");
  }

  return NULL;
}

}  // namespace toml98