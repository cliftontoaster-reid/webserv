#include <sstream>
#include <stdexcept>
#include <vector>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Value.hpp"

namespace toml98 {

void Parser::readKeyPair(std::vector<PathPart> root) {
  std::vector<PathPart> path = readKeyPath(TokenEqual);
  if (path.empty()) {
    throw std::runtime_error("Path cannot be empty.");
  }

  skipWhitespace();
  root.insert(root.end(), path.begin(), path.end() - 1);
  readValue(root, path.back());
  skipWhitespace();
}

}  // namespace toml98