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
      if (isWordBoolean(tok)) {
        insertOrDie(root, Value::createBoolean(readBoolean(tok)));
      } else if (isWordFloat(tok)) {
        insertOrDie(root, Value::createFloat(readFloat(tok)));
      } else if (isWordDigit(tok)) {
        insertOrDie(root, Value::createInteger(readInteger(tok)));
      } else {
        throw std::runtime_error("Invalid word in readValue.");
      }
      break;

    case TokenString:
    case TokenMultiString:
      insertOrDie(root, Value::createString(tok.value));
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