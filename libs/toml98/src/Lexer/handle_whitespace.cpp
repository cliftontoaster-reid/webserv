#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_whitespace() {
  char now = 0;

  while ((now = peek()) != '\0' && (now == ' ' || now == '\t')) {
    _buffer.push_back(now);
    pop();
  }

  _stack.pop();
  if (_buffer.empty()) {
    return NULL;
  }
  std::string tmp;
  tmp.swap(_buffer);
  return new Token(TokenDelimiter, tmp);
}

}  // namespace toml98