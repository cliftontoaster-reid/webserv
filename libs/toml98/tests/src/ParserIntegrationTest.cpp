#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <stdint.h>

#include <fstream>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "StupidLexer.hpp"
#include "StupidParser.hpp"
#include "Value.hpp"

namespace toml98 {

std::string read_file(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + path);
  }
  return std::string(std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>());
}

std::vector<toml98::Token> tokenize_all(const std::string& input) {
  toml98::StupidLexer lex(input);
  std::vector<toml98::Token> tokens;

  while (lex.stupid_remaining() > 0) {
    try {
      Token* res = lex.run();
      if (res == NULL) {
        continue;
      }

      tokens.push_back(*res);
      delete res;
    } catch (const std::runtime_error&) {
      break;
    }
  }
  return tokens;
}

Test(parser_integration, official_example_without_date_toml) {
  std::string content = read_file("assets/official_example_no_date.toml");
  std::vector<toml98::Token> tokens = tokenize_all(content);

  StupidParser par;
  Value res = par.stupid_parse(tokens);

  check_entry(res, "TOML Example", "title");

  check_entry(res, "Tom Preston-Werner", "owner", "name");

  check_entry(res, true, "database", "enabled");
  check_entry(res, 8000, "database", "ports", 0);
  check_entry(res, 8001, "database", "ports", 1);
  check_entry(res, 8002, "database", "ports", 2);

  check_entry(res, "delta", "database", "data", 0, 0);
  check_entry(res, "phi", "database", "data", 0, 1);
  check_entry(res, 3.14, "database", "data", 1, 0);

  check_entry(res, "10.0.0.1", "servers", "alpha", "ip");
  check_entry(res, "frontend", "servers", "alpha", "role");

  check_entry(res, "10.0.0.2", "servers", "beta", "ip");
  check_entry(res, "backend", "servers", "beta", "role");
}

}  // namespace toml98
