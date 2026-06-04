#include "Parser.hpp"

#include <stdexcept>
#include <string>
#include <vector>

#include "Lexer.hpp"

namespace toml98 {

void Parser::pop() { pop(1); }
void Parser::pop(u_int64_t amount) {
  if (_pos + amount > _data.size()) {
    throw std::runtime_error("Early end of file.");
  }
  _pos += amount;
}
const Token& Parser::peek() const { return peek(0); }
const Token& Parser::peek(u_int64_t offset) const {
  if ((_pos + offset) >= _data.size()) {
    throw std::runtime_error("Early end of file.");
  }
  return _data.at(_pos + offset);
}

bool Parser::canPeek() { return (_pos < _data.size()); }

const Token& Parser::consume(const std::string& msg) {
  if (!canPeek()) {
    throw std::runtime_error("Early End Of File in " + msg + ".");
  }
  const Token& tok = peek();
  pop();

  return tok;
}

void Parser::skipWhitespace() {
  while (canPeek()) {
    const Token& tok = peek();
    if (tok.type != TokenDelimiter) {
      break;
    }

    pop();
  }
}

}  // namespace toml98