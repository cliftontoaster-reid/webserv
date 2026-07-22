#include <utility>
#include <vector>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Value.hpp"

namespace toml98 {

void Parser::readSuperKey(bool isArray) {
  if (peek().type == TokenArrayKeyStart || peek().type == TokenTableKeyStart) {
    pop();
  }
  std::vector<PathPart> ret =
      readKeyPath(isArray ? TokenArrayKeyEnd : TokenTableKeyEnd);

  if (isArray) {
    std::map<std::vector<PathPart>, int64_t>::iterator it = _depthMap.find(ret);
    if (it != _depthMap.end()) {
      for (std::map<std::vector<PathPart>, int64_t>::iterator dit =
               _depthMap.begin();
           dit != _depthMap.end();) {
        bool isSubPath = dit->first.size() > ret.size();
        for (std::size_t i = 0; isSubPath && i < ret.size(); ++i) {
          const PathPart& a = dit->first[i];
          const PathPart& b = ret[i];
          if (a.type != b.type || (a.type == PathPart::PathPartKey && a.key != b.key) ||
              (a.type == PathPart::PathPartIndex && a.index != b.index)) {
            isSubPath = false;
          }
        }
        if (isSubPath) {
          _depthMap.erase(dit++);
        } else {
          ++dit;
        }
      }
      ret.push_back(PathPart::makeIndex(it->second++));
    } else {
      _depthMap.insert(std::make_pair(ret, 1));
      ret.push_back(PathPart::makeIndex(0));
    }
  }
  insertOrDie(ret, Value::createTable(std::map<std::string, Value>()));

  readBody(ret);
}

}  // namespace toml98
