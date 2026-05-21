#include <cctype>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_word() {
  char letter = peek();
  while (std::isalnum(letter) != 0) {
    pop();
    _buffer.push_back(letter);
    letter = peek();
  }
  std::string tmp;
  tmp.swap(_buffer);
  return new Token(TokenWord, tmp);
}

}  // namespace toml98
