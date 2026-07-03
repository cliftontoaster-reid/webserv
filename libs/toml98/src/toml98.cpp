#include "toml98.hpp"

#include <cstddef>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"
namespace toml98 {

toml98::Value readTomlFile(const char* path) {
  std::ifstream ifs(path);
  if (!ifs.is_open()) {
    throw std::runtime_error("toml98: could not open file: " +
                             std::string(path));
  }

  std::stringstream sstream;
  sstream << ifs.rdbuf();
  if (ifs.bad()) {
    throw std::runtime_error("toml98: error reading file: " +
                             std::string(path));
  }

  std::string content = sstream.str();
  if (content.empty()) {
    throw std::runtime_error("toml98: empty file: " + std::string(path));
  }

  toml98::Lexer lexer(content);
  std::vector<toml98::Token> tokens;
  toml98::Token* tok = NULL;
  while ((tok = lexer.run()) != NULL) {
    tokens.push_back(*tok);
    delete tok;
  }

  toml98::Parser parser;
  return parser.parse(tokens);
}

}  // namespace toml98