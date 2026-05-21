#include <stdexcept>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_string() {
  if (!_buffer.empty()) {
    _buffer.clear();
    throw std::runtime_error(
        "Literal string started right with a non empty buffer.");
  }

  while (_pos < _source.length()) {
    char now = peek();
    pop();

    if (now == '\n' || now == '\r') {
      _buffer.clear();
      throw std::runtime_error(
          "Unexpected new line in non multi-line literal string.");
    }

    if (now == '\'') {
      std::string tmp;
      tmp.swap(_buffer);
      return new Token(TokenDelimiter, tmp);
    }

    _buffer.push_back(now);
  }

  _buffer.clear();
  throw std::runtime_error("Unterminated literal string.");
}

}  // namespace toml98