#include "Parser.hpp"
#include "Value.hpp"

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

void Parser::readArray(const std::vector<PathPart>& root) {
  int64_t pos = 0;

  while (canPeek()) {
    skipWhitespaceAndNewlines(_data, _pos);
    if (!canPeek()) {
      throw std::runtime_error("Unterminated inline array");
    }

    const Token& tok = peek();
    if (tok.type == TokenArrayEnd) {
      break;
    }

    readValue(root, PathPart::makeIndex(pos++));
    skipWhitespaceAndNewlines(_data, _pos);

    const Token& next = peek();
    if (next.type == TokenComma) {
      pop();
    } else if (next.type == TokenArrayEnd) {
      break;
    } else {
      throw std::runtime_error("Expected comma or ] in inline array");
    }
  }

  pop();
}

}  // namespace toml98