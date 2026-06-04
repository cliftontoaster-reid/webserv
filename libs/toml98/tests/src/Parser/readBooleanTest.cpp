#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <stdint.h>

#include <stdexcept>
#include <vector>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "StupidParser.hpp"

Test(readBoolean, valid_true) {
  toml98::Token tok = toml98::Token(toml98::TokenWord, "true");
  toml98::StupidParser parser;

  bool ret = parser.stupid_readBoolean(tok);

  cr_expect_eq(ret, true);
}

Test(readBoolean, valid_false) {
  toml98::Token tok = toml98::Token(toml98::TokenWord, "false");
  toml98::StupidParser parser;

  bool ret = parser.stupid_readBoolean(tok);

  cr_expect_eq(ret, false);
}

Test(readBoolean, invalid) {
  toml98::Token tok = toml98::Token(toml98::TokenWord, "permanent");
  toml98::StupidParser parser;

  cr_expect_throw(parser.stupid_readBoolean(tok), std::runtime_error);
}
