#include <sys/types.h>

#include <stdexcept>
#include <string>

#include "Lexer.hpp"

static inline u_int64_t getSpecialLenght(char code) {
  switch (code) {
    case 'b':
    case 't':
    case 'n':
    case 'f':
    case 'r':
    case 'e':
    case '"':
    case '\\':
      return UTF8_ESCLEN_SPECIAL;
    case 'x':
      return UTF8_ESCLEN_X;
    case 'u':
      return UTF8_ESCLEN_U;
    case 'U':
      return UTF8_ESCLEN_UU;
    default:
      throw std::runtime_error("Unexpected special character.");
  }
}

namespace toml98 {

Token* Lexer::handle_string_double() {
  std::string tmp;

  while (true) {
    char now = peek();
    pop();

    switch (now) {
      case '"':
        _stack.pop();
        return new Token(TokenString, tmp);

      case '\\': {
        if (!canPeekAt(1)) {
          throw std::runtime_error("Unknown character in Normal");
        }
        u_int64_t len = getSpecialLenght(peek(1));
        if (!canPeekAt(len)) {
          throw std::runtime_error("Unknown character in Normal");
        }

        if (len == 1) {
          pop();
          tmp.push_back(getSpecial(peek()));
        } else {
          std::string code = peek(0, len);
          tmp.append(getUnicode(code));
          pop(len);
        }
      }

      default:
        tmp.push_back(now);
        break;
    }
  }
}

}  // namespace toml98