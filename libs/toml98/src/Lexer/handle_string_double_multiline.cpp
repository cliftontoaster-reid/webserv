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
#include <string>

#include "Lexer.hpp"

namespace toml98 {

Token* Lexer::handle_string_double_multiline() {
  std::string result;

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

    switch (now) {
      case '"':
        if (handleQuoteSequence(result, '"')) {
          return new Token(TokenString, result);
        }
        break;

      case '\\':
        handleEscapeSequence(result);
        break;

      default:
        result.push_back(now);
    }
  }

  throw std::runtime_error("Unterminated multiline string.");
}

}  // namespace toml98
