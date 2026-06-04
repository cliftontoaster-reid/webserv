#include <stdexcept>

#include "Parser.hpp"
#include "Value.hpp"
#include "utils.hpp"

namespace toml98 {

void Parser::readValue(std::vector<PathPart> root, const PathPart& key) {
  if (!canPeek()) {
    throw std::runtime_error("Early End Of File in readValue.");
  }
  const Token& tok = peek();
  pop();

  root.push_back(key);

  switch (tok.type) {
    case TokenWord:
      if (_isWordBoolean(tok)) {
        _document->insertOrDie(root, Value::createBoolean(readBoolean(tok)));
      } else if (_isWordFloat(tok)) {
        _document->insertOrDie(root, Value::createFloat(readFloat(tok)));
      } else if (_isWordDigit(tok)) {
        _document->insertOrDie(root, Value::createInteger(readInteger(tok)));
      } else {
        throw std::runtime_error("Invalid word in readValue.");
      }
      break;

    case TokenString:
    case TokenMultiString:
      _document->insertOrDie(root, Value::createString(tok.value));
      break;

    case TokenTableStart:
      readTable(root);
      break;
    case TokenArrayStart:
      readArray(root);
      break;
    case TokenTableKeyStart:
    case TokenTableKeyEnd:
    case TokenArrayKeyStart:
    case TokenArrayKeyEnd:
    case TokenArrayEnd:
    case TokenTableEnd:
    case TokenEqual:
    case TokenComma:
    case TokenDelimiter:
    case TokenNewLine:
    case TokenDot:
      throw std::runtime_error("Unexpected token in readValue : " +
                               tokTypeToString(tok.type));
  }
}

}  // namespace toml98