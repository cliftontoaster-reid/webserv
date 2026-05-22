/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_string_double_multiline.cpp                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfranc <jfranc@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 15:10:23 by jfranc            #+#    #+#             */
/*   Updated: 2026/05/22 15:55:57 by jfranc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdexcept>

#include "Exception.hpp"
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

Token* Lexer::handle_string_double_multiline() {
  if (!_buffer.empty()) {
    _buffer.clear();
    throw std::runtime_error(
        "Literal string started right with a non empty buffer.");
  }

  if (canPeek('\n')) {
    pop();
  } else if (canPeek('\r')) {
    pop();
    if (canPeek('\n')) {
      pop();
    }
  }

  while (_pos < _source.length()) {
    char now = peek();
    pop();

    if (now == '"') {
      std::size_t quoteCount = 1;
      while (_pos < _source.length() && peek() == '"') {
        quoteCount++;
        pop();
      }

      if (quoteCount >= 3) {
        if (quoteCount > 3) {
          _buffer.append(quoteCount - 3, '"');
        }

        std::string tmp;
        tmp.swap(_buffer);
        return new Token(TokenDelimiter, tmp);
      }
      _buffer.append(quoteCount, '"');
    } else if (now == '\\') {
      if (!canPeekAt(1)) {
        throw std::runtime_error("Unknown character in Normal");
      }
      u_int64_t len = getSpecialLenght(peek(1));
      if (!canPeekAt(len)) {
        throw std::runtime_error("Unknown character in Normal");
      }

      if (len == 1) {
        pop();
        _buffer.push_back(getSpecial(peek()));
      } else {
        std::string code = peek(0, len);
        _buffer.append(getUnicode(code));
        pop(len);
      }
    }
  }

  _buffer.clear();
  throw std::runtime_error("Unterminated literal multiline string.");
}

}  // namespace toml98
