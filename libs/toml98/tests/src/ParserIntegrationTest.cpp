#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <stdint.h>

#include <cstdio>
#include <fstream>
#include <iostream>
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

  check_entry(__FILE__, __LINE__, res, "TOML Example", "title");

  check_entry(__FILE__, __LINE__, res, "Tom Preston-Werner", "owner", "name");

  check_entry(__FILE__, __LINE__, res, true, "database", "enabled");
  check_entry(__FILE__, __LINE__, res, 8000, "database", "ports", 0);
  check_entry(__FILE__, __LINE__, res, 8001, "database", "ports", 1);
  check_entry(__FILE__, __LINE__, res, 8002, "database", "ports", 2);

  check_entry(__FILE__, __LINE__, res, "delta", "database", "data", 0, 0);
  check_entry(__FILE__, __LINE__, res, "phi", "database", "data", 0, 1);
  check_entry(__FILE__, __LINE__, res, 3.14, "database", "data", 1, 0);

  check_entry(__FILE__, __LINE__, res, "10.0.0.1", "servers", "alpha", "ip");
  check_entry(__FILE__, __LINE__, res, "frontend", "servers", "alpha", "role");

  check_entry(__FILE__, __LINE__, res, "10.0.0.2", "servers", "beta", "ip");
  check_entry(__FILE__, __LINE__, res, "backend", "servers", "beta", "role");
}

Test(parser_integration, config) {
  std::string content = read_file("assets/config.toml");
  std::vector<toml98::Token> tokens = tokenize_all(content);

  StupidParser par;
  Value res = par.stupid_parse(tokens);

  check_entry(__FILE__, __LINE__, res, 80, "server", 0, "ports", 0);

  check_entry(__FILE__, __LINE__, res, "/srv/http/404.html", "server", 0,
              "errors", "404", "fs");

  check_entry(__FILE__, __LINE__, res, "/", "server", 0, "endpoints", 0,
              "path");
  check_entry(__FILE__, __LINE__, res, "/srv/httpd/", "server", 0, "endpoints",
              0, "fs");

  check_entry(__FILE__, __LINE__, res, "/static", "server", 0, "endpoints", 1,
              "path");
  check_entry(__FILE__, __LINE__, res, "/srv/data/", "server", 0, "endpoints",
              1, "fs");

  check_entry(__FILE__, __LINE__, res, ".php", "server", 0, "cgi", 0, "ext");
  check_entry(__FILE__, __LINE__, res, "/", "server", 0, "cgi", 0, "if");
  check_entry(__FILE__, __LINE__, res, "/php/path/here", "server", 0, "cgi", 0,
              "bin");

  check_entry(__FILE__, __LINE__, res, ".page.js", "server", 0, "cgi", 1,
              "ext");
  check_entry(__FILE__, __LINE__, res, "/static", "server", 0, "cgi", 1,
              "ifnot");
  check_entry(__FILE__, __LINE__, res, "/nodejs/path/here", "server", 0, "cgi",
              1, "bin");
}

}  // namespace toml98
