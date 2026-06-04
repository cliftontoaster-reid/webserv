#include <stdexcept>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Value.hpp"
#include "utils.hpp"

namespace toml98 {

std::vector<PathPart> Parser::readKeyPath(TokenType superMario) {
  bool isLastDot = true;
  std::vector<PathPart> ret = std::vector<PathPart>();

  while (canPeek()) {
    const Token& tok = peek();

    switch (tok.type) {
      case TokenString:
      case TokenWord:
        if (!isLastDot) {
          throw std::runtime_error("Invalid path format.");
        }
        ret.push_back(PathPart::makeKey(tok.value));
        isLastDot = false;
        break;
      case TokenDot:
        if (isLastDot) {
          throw std::runtime_error("Invalid path format.");
        }
        isLastDot = true;
        break;
      case TokenDelimiter:
        break;

      case TokenEqual:
      case TokenTableKeyEnd:
      case TokenArrayKeyEnd:
        if (superMario == tok.type) {
          return ret;
        } else {
          throw std::runtime_error(
              "Unexpected token in readKeyPath : " + tokTypeToString(tok.type) +
              " = '" + tok.value + "'");
        }

      case TokenMultiString:
      case TokenTableKeyStart:
      case TokenArrayKeyStart:
      case TokenArrayStart:
      case TokenArrayEnd:
      case TokenTableStart:
      case TokenTableEnd:
      case TokenComma:
      case TokenNewLine:
        throw std::runtime_error(
            "Unexpected token in readKeyPath : " + tokTypeToString(tok.type) +
            " = '" + tok.value + "'");
    }

    pop();
  }

  throw std::runtime_error("Early End Of File in KeyPath.");
}

}  // namespace toml98