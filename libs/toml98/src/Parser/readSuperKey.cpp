#include <vector>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Value.hpp"

namespace toml98 {

void Parser::readSuperKey(bool isArray) {
  std::vector<PathPart> ret =
      readKeyPath(isArray ? TokenArrayKeyEnd : TokenTableKeyEnd);

  if (isArray) {
    if (_document->has(ret)) {
      const Value& val = _document->get(ret);
      if (val.type() != ValueArray) {
        throw std::runtime_error("Expected array for array table.");
      }
      ret.push_back(PathPart::makeIndex(val.getArray()->size()));
    } else {
      ret.push_back(PathPart::makeIndex(0));
    }
  }
  _document->insertOrDie(ret,
                         Value::createTable(std::map<std::string, Value>()));

  readBody(ret);
}

}  // namespace toml98
