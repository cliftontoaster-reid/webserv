#include "Parser.hpp"

namespace toml98 {

static inline void skipWhitespaceAndNewlines(const std::vector<Token>& data,
                                             u_int64_t& pos) {
  while (pos < data.size()) {
    const Token& tok = data[pos];
    if (tok.type != TokenDelimiter && tok.type != TokenNewLine) {
      break;
    }
    pos++;
  }
}

void Parser::readTable(const std::vector<PathPart>& root) {
  while (canPeek()) {
    skipWhitespaceAndNewlines(_data, _pos);
    if (!canPeek()) {
      throw std::runtime_error("Unterminated inline table");
    }

    const Token& tok = peek();
    if (tok.type == TokenTableEnd) {
      break;
    }

    readKeyPair(root);
    skipWhitespaceAndNewlines(_data, _pos);

    const Token& next = peek();
    if (next.type == TokenComma) {
      pop();
    } else if (next.type == TokenTableEnd) {
      break;
    } else {
      throw std::runtime_error("Expected comma or } in inline table");
    }
  }

  pop();
}

}  // namespace toml98