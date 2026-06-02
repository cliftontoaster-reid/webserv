#include <stdexcept>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_inline_table() {
  char now = peek();

  switch (now) {
    case '#':
      pop();
      _stack.push(LexerComments);
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

    case '{':
      pop();
      _stack.push(LexerInlineTable);
      return new Token(TokenTableStart, "{");

    case '}':
      pop();
      _stack.pop();
      return new Token(TokenTableEnd, "}");

    case '[': {
      pop();
      _stack.push(LexerInlineArray);
      return new Token(TokenArrayStart, "[");
    }

    case '=':
      pop();
      return new Token(TokenEqual, "=");

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
      if (canPeek('\n', 1)) {
        pop(2);
        return new Token(TokenNewLine, "\r\n");
      }
      throw std::runtime_error(
          "TOML does not support Carriage Return new lines, "
          "change it to '\\n' (Linux) or '\\r\\n' (Windows)");

    case ',':
      pop();
      return new Token(TokenComma, ",");

    default: {
      // A-Za-z0-9_- (see the last two checks bellow)
      if (std::isalnum(now) != 0) {
        _stack.push(LexerWord);
        return NULL;
      }
      throw std::runtime_error("Unknown character in Normal");
    }
  }

  return NULL;
}

}  // namespace toml98