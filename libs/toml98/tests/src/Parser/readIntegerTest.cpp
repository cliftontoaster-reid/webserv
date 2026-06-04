#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <stdint.h>

#include <stdexcept>
#include <vector>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "StupidParser.hpp"

Test(readInteger, valid) {
  toml98::Token tok = toml98::Token(toml98::TokenWord, "69");

  std::vector<toml98::Token> tokens;
  tokens.push_back(tok);

  toml98::StupidParser parser;

  int64_t ret = parser.stupid_readInteger(tok);

  cr_expect_eq(ret, 69);
}

Test(readInteger, invalid) {
  toml98::Token tok =
      toml98::Token(toml98::TokenWord, "spaghetti bolognese Papyrus");

  std::vector<toml98::Token> tokens;
  tokens.push_back(tok);

  toml98::StupidParser parser;

  cr_expect_throw(parser.stupid_readInteger(tok), std::runtime_error);
}

Test(readInteger, invalid_string) {
  // https://www.youtube.com/watch?v=pvy9km7g6fw
  toml98::Token tok = toml98::Token(
      toml98::TokenMultiString,
      "I am the storm that is approaching\nProvoking black clouds in "
      "isolation\nI am reclaimer of my name\nBorn in flames, I have been "
      "blessed\nMy family crest is a demon of death!");

  std::vector<toml98::Token> tokens;
  tokens.push_back(tok);

  toml98::StupidParser parser;

  cr_expect_throw(parser.stupid_readInteger(tok), std::runtime_error);
}
