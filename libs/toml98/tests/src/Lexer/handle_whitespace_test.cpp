#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>

#include "StupidLexer.hpp"

namespace toml98 {

static void expect_delimiter_token(Token* token, const std::string& expected) {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, TokenDelimiter);
  cr_assert_eq(token->value, expected);
  delete token;
}

}  // namespace toml98

Test(lexer_handle_whitespace, single_space) {
  toml98::StupidLexer stupid(" ");

  toml98::Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);

  token = stupid.stupid_handle_whitespace();
  toml98::expect_delimiter_token(token, " ");

  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
}

Test(lexer_handle_whitespace, mixed_whitespace) {
  toml98::StupidLexer stupid(" \t \t ");

  toml98::Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);

  token = stupid.stupid_handle_whitespace();
  toml98::expect_delimiter_token(token, " \t \t ");

  cr_expect_eq(stupid.stupid_pos(), 5U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
}

Test(lexer_handle_whitespace, tabs_only) {
  toml98::StupidLexer stupid("\t\t");

  toml98::Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);

  token = stupid.stupid_handle_whitespace();
  toml98::expect_delimiter_token(token, "\t\t");

  cr_expect_eq(stupid.stupid_pos(), 2U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
}

Test(lexer_handle_whitespace, no_whitespace_returns_null) {
  toml98::StupidLexer stupid("a");

  stupid.stupid_stack().push(toml98::LexerNormal);
  stupid.stupid_stack().push(toml98::LexerWhiteSpace);

  toml98::Token* token = stupid.stupid_handle_whitespace();

  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
}
