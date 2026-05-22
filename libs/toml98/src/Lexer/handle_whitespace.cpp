#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_whitespace() {
  while (true) {
    char now = peek();
    if (now == '\0' || (now != ' ' && now != '\t')) {
      break;
    }

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