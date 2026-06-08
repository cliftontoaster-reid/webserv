
#include "Lexer.hpp"
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

void Parser::readBody(const std::vector<PathPart>& root) {
  while (canPeek()) {
    skipWhitespaceAndNewlines(_data, _pos);

    if (!canPeek()) {
      break;
    }

    const Token& tok = peek();
    if (tok.type == TokenTableKeyStart || tok.type == TokenArrayKeyStart) {
      return;
    }

    readKeyPair(root);
  }
}

}  // namespace toml98
