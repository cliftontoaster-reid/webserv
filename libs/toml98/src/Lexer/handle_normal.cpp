#include <sys/types.h>

#include <stdexcept>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_normal() {
  char letter = peek();

  // A-Za-z0-9_- (see the last two checks bellow)
  if (std::isalnum(letter) != 0) {
    _stack.push(LexerWord);
    return NULL;
  }
  switch (letter) {
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
    case '"': {
      pop();

      u_int64_t rem = remaining();
      if (rem >= 2 && peek() == '"' && peek(1) == '"') {
        pop(2);
        _stack.push(LexerStringDoubleMultiLine);
      } else if (rem > 0) {
        _stack.push(LexerStringDouble);
      } else {
        throw std::runtime_error("Early end of file.");
      }

      break;
    }
    case '\'': {
      pop();

      u_int64_t rem = remaining();
      if (rem >= 2 && peek() == '\'' && peek(1) == '\'') {
        pop(2);
        _stack.push(LexerStringMultiLine);
      } else if (rem > 0) {
        _stack.push(LexerString);
      } else {
        throw std::runtime_error(
            "Unterminated literal string: Early end of file.");
      }

      break;
    }
    case '[': {
      pop();
      if (peek() != '[') {
        _stack.push(LexerTableKey);
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
      throw std::runtime_error(
          "Unterminated literal string: Unknown character in Normal");
  }

  return NULL;
}

}  // namespace toml98