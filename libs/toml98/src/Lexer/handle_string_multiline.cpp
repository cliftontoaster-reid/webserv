/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_string_multiline.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jfranc <jfranc@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 15:11:21 by jfranc            #+#    #+#             */
/*   Updated: 2026/05/22 15:11:23 by jfranc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdexcept>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_string_multiline() {
  if (!_buffer.empty()) {
    _buffer.clear();
    throw std::runtime_error("Literal string started with a non-empty buffer.");
  }

  if (canPeek('\n')) {
    pop();
  } else if (canPeek('\r')) {
    pop();
    if (canPeek('\n')) {
      pop();
    }
  }

  while (canPeek()) {
    char now = peek();
    pop();

    if (now == '\'') {
      std::size_t quoteCount = 1;
      while (canPeek('\'')) {
        quoteCount++;
        pop();
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
