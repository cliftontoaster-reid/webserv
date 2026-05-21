#include <stdexcept>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_string_multiline() {
  if (!_buffer.empty()) {
    _buffer.clear();
    throw std::runtime_error(
        "Literal string started right with a non empty buffer.");
  }

  if (_pos < _source.length() && _source.at(_pos) == '\n') {
    pop();
  } else if (_pos < _source.length() && _source.at(_pos) == '\r') {
    pop();
    if (_pos < _source.length() && _source.at(_pos) == '\n') {
      pop();
    }
  }

  while (_pos < _source.length()) {
    char now = pop();

    if (now == '\'') {
      std::size_t quoteCount = 1;
      while (_pos < _source.length() && pop() == '\'') {
        quoteCount++;
      }

      if (quoteCount >= 3) {
        if (quoteCount > 3) {
          _buffer.append(quoteCount - 3, '\'');
        }

        std::string tmp;
        tmp.swap(_buffer);
        return new Token(TokenDelimiter, tmp);
      }
      _buffer.append(quoteCount, '\'');
    }
  }

  _buffer.clear();
  throw std::runtime_error("Unterminated literal multiline string.");
}

}  // namespace toml98