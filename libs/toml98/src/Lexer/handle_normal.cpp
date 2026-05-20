#include <stdexcept>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_normal() {
  char letter = peek();

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
      if (peek() != '"') {
        _stack.push(LexerStringDouble);
      } else {
        pop();
        if (pop() == '"') {
          _stack.push(LexerStringDoubleMultiLine);
        } else {
          throw std::runtime_error("Cannot use two quotes together");
        }
      }
      break;
    }
    case '\'': {
      pop();
      if (peek() != '\'') {
        _stack.push(LexerString);
      } else {
        pop();
        if (pop() == '\'') {
          _stack.push(LexerStringMultiLine);
        } else {
          throw std::runtime_error("Cannot use two quotes together");
        }
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
    default:
      throw std::runtime_error("Unknown character in Normal");
  }

  return NULL;
}

}  // namespace toml98