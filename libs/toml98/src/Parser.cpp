#include "Parser.hpp"

#include <algorithm>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "Value.hpp"

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
bool Parser::canPeek(u_int64_t offset) {
  return (_pos + offset < _data.size());
}

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

Value Parser::parse(std::vector<Token> tokens) {
  _data.clear();
  _data.swap(tokens);
  _pos = 0;
  _document = new Value(Value::createTable(std::map<std::string, Value>()));

  while (canPeek()) {
    readBody(std::vector<PathPart>());
    if (!canPeek()) {
      break;
    }
    const Token& tok = peek();
    if (tok.type == TokenTableKeyStart || tok.type == TokenArrayKeyStart) {
      readSuperKey(tok.type == TokenArrayKeyStart);
    }
  }

  Value result = *_document;
  delete _document;
  _document = NULL;
  return result;
}

void Parser::insertOrDie(const std::vector<PathPart>& path,
                         const Value& value) {
  std::vector<PathPart> resolved(path);
  resolved.reserve(path.size());

  for (std::size_t len = path.size(); len > 0; --len) {
    std::vector<PathPart> prefix(path.begin(), path.begin() + len);
    std::map<std::vector<PathPart>, int64_t>::iterator it =
        _depthMap.find(prefix);
    if (it != _depthMap.end()) {
      if (len == path.size()) {
        throw std::runtime_error("Path is already an array table.");
      }
      resolved.insert(resolved.begin() + len, PathPart::makeIndex(it->second));
    }
  }

  _document->insertOrDie(path, value);
}

}  // namespace toml98