#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"

namespace toml98 {

double Parser::readFloat(const Token& tok) {
  if (!isWordFloat(tok)) {
    throw std::runtime_error("Invalid floating point number : " + tok.value);
  }

  std::string tmp;
  tmp.append(tok.value);
  tmp.push_back('.');
  pop();
  const Token& tok2 = consume("readFloat");
  tmp.append(tok2.value);

  return std::strtod(tmp.c_str(), NULL);
}

}  // namespace toml98
