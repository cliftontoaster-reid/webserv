#include <cctype>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_word() {
  while (true) {
    char now = peek();
    if (std::isalnum(now) == 0 && now != '-' && now != '_') {
      break;
    }
    pop();
    _buffer.push_back(now);
  }

  std::string tmp;
  tmp.swap(_buffer);
  return new Token(TokenWord, tmp);
}

}  // namespace toml98
