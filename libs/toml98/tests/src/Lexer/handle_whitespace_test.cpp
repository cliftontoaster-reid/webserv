#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <stack>
#include <stdexcept>
#include <string>
#include "StupidLexer.hpp"

using namespace toml98;

static void expect_whitespace_token(Token* token, const std::string& expected_ws) {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, TokenDelimiter);
  cr_assert_eq(token->value, expected_ws);
  delete token;
}

// Normal Tests

Test(lexer_handle_whitespace, single_space) {
  StupidLexer stupid(" x");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, " ");
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_whitespace, multiple_spaces) {
  StupidLexer stupid("    x");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "    ");
  cr_assert_eq(stupid.stupid_pos(), 4);
}

Test(lexer_handle_whitespace, single_tab) {
  StupidLexer stupid("\tx");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "\t");
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_whitespace, multiple_tabs) {
  StupidLexer stupid("\t\t\tx");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "\t\t\t");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_whitespace, mixed_space_tab) {
  StupidLexer stupid(" \t  \t x");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, " \t  \t ");
  cr_assert_eq(stupid.stupid_pos(), 7);
}

// Negative/Boundary Tests

Test(lexer_handle_whitespace, stops_at_alphanumeric) {
  StupidLexer stupid("   a");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "   ");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_whitespace, stops_at_eof) {
  StupidLexer stupid("   ");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "   ");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_whitespace, no_leading_whitespace) {
  StupidLexer stupid("a");
  Token* token = stupid.stupid_handle_whitespace();
  cr_assert_null(token);
}

Test(lexer_handle_whitespace, stops_at_newline) {
  StupidLexer stupid("   \n");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "   ");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_whitespace, stops_at_hash) {
  StupidLexer stupid("  #");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "  ");
  cr_assert_eq(stupid.stupid_pos(), 2);
}

Test(lexer_handle_whitespace, stops_at_bracket) {
  StupidLexer stupid("   [");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "   ");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_whitespace, stops_at_brace) {
  StupidLexer stupid("   {");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "   ");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_whitespace, very_long_whitespace) {
  std::string input(10000, ' ');
  input += 'x';
  StupidLexer stupid(input);
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, std::string(10000, ' '));
  cr_assert_eq(stupid.stupid_pos(), 10000);
}

Test(lexer_handle_whitespace, stops_at_quote) {
  StupidLexer stupid("   \"");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "   ");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_whitespace, stops_at_equals) {
  StupidLexer stupid("   =");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "   ");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_whitespace, stops_at_colon) {
  StupidLexer stupid("   :");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "   ");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_whitespace, stops_at_dot) {
  StupidLexer stupid("   .");
  Token* token = stupid.stupid_handle_whitespace();
  expect_whitespace_token(token, "   ");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_whitespace, empty_source) {
  StupidLexer stupid("");
  Token* token = stupid.stupid_handle_whitespace();
  cr_assert_null(token);
}
