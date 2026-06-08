#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>
#include <string>

#include "StupidLexer.hpp"

namespace toml98 {

static void expect_token(Token* token, TokenType type,
                         const std::string& value) {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, type);
  cr_assert_eq(token->value, value);
  delete token;
}

static void enter_array_key(StupidLexer& stupid) {
  Token* token = stupid.stupid_handle_normal();
  expect_token(token, TokenArrayKeyStart, "[[");

  auto state = stupid.stupid_stack();
  cr_assert_not(state.empty());
  cr_assert_eq(state.top(), LexerArrayKey);
}

}  // namespace toml98

Test(lexer_handle_array_key, alnum_starts_word_state) {
  toml98::StupidLexer stupid("[[abc");

  toml98::enter_array_key(stupid);

  toml98::Token* token = stupid.stupid_handle_array_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 2U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}

Test(lexer_handle_array_key, hyphen_starts_word_state) {
  toml98::StupidLexer stupid("[[a-b");

  toml98::enter_array_key(stupid);

  toml98::Token* token = stupid.stupid_handle_array_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 2U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}

Test(lexer_handle_array_key, underscore_starts_word_state) {
  toml98::StupidLexer stupid("[[a_b");

  toml98::enter_array_key(stupid);

  toml98::Token* token = stupid.stupid_handle_array_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 2U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}

Test(lexer_handle_array_key, dot_token) {
  toml98::StupidLexer stupid("[[a.b]]");

  toml98::enter_array_key(stupid);

  toml98::Token* token = stupid.stupid_handle_array_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 2U);

  token = stupid.stupid_handle_word();
  expect_token(token, toml98::TokenWord, "a");
  cr_expect_eq(stupid.stupid_pos(), 3U);

  token = stupid.stupid_handle_array_key();
  expect_token(token, toml98::TokenDot, ".");
  cr_expect_eq(stupid.stupid_pos(), 4U);
}

Test(lexer_handle_array_key, array_key_end) {
  toml98::StupidLexer stupid("[[a]]");

  toml98::enter_array_key(stupid);

  toml98::Token* token = stupid.stupid_handle_array_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 2U);

  token = stupid.stupid_handle_word();
  expect_token(token, toml98::TokenWord, "a");
  cr_expect_eq(stupid.stupid_pos(), 3U);

  token = stupid.stupid_handle_array_key();
  expect_token(token, toml98::TokenArrayKeyEnd, "]");
  cr_expect_eq(stupid.stupid_pos(), 5U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
}

Test(lexer_handle_array_key, single_right_bracket_throws) {
  toml98::StupidLexer stupid("[[a]");

  toml98::enter_array_key(stupid);

  toml98::Token* token = stupid.stupid_handle_array_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 2U);

  token = stupid.stupid_handle_word();
  expect_token(token, toml98::TokenWord, "a");
  cr_expect_eq(stupid.stupid_pos(), 3U);

  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, nested_array_key_throws) {
  toml98::StupidLexer stupid("[[a[[b]]");

  toml98::enter_array_key(stupid);

  toml98::Token* token = stupid.stupid_handle_array_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 2U);

  token = stupid.stupid_handle_word();
  expect_token(token, toml98::TokenWord, "a");
  cr_expect_eq(stupid.stupid_pos(), 3U);

  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, double_quote_string_state) {
  toml98::StupidLexer stupid("[[\"a\"]]");

  toml98::enter_array_key(stupid);

  toml98::Token* token = stupid.stupid_handle_array_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 3U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerStringDouble);
}

Test(lexer_handle_array_key, single_quote_string_state) {
  toml98::StupidLexer stupid("[[\'a\']]");

  toml98::enter_array_key(stupid);

  toml98::Token* token = stupid.stupid_handle_array_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 3U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerString);
}

Test(lexer_handle_array_key, multiline_double_quote_throws) {
  toml98::StupidLexer stupid("[[\"\"\"a]]");

  toml98::enter_array_key(stupid);

  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, multiline_single_quote_throws) {
  toml98::StupidLexer stupid("[[\'\'\'a]]");

  toml98::enter_array_key(stupid);

  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, unterminated_quote_throws) {
  toml98::StupidLexer stupid("[[\"");

  toml98::enter_array_key(stupid);

  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, whitespace_pushes_state) {
  toml98::StupidLexer stupid("[[ ");

  toml98::enter_array_key(stupid);

  toml98::Token* token = stupid.stupid_handle_array_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 2U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWhiteSpace);

  token = stupid.stupid_handle_whitespace();
  expect_token(token, toml98::TokenDelimiter, " ");
  cr_expect_eq(stupid.stupid_pos(), 3U);
}

Test(lexer_handle_array_key, tab_pushes_state) {
  toml98::StupidLexer stupid("[[\t");

  toml98::enter_array_key(stupid);

  toml98::Token* token = stupid.stupid_handle_array_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 2U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWhiteSpace);

  token = stupid.stupid_handle_whitespace();
  expect_token(token, toml98::TokenDelimiter, "\t");
  cr_expect_eq(stupid.stupid_pos(), 3U);
}

Test(lexer_handle_array_key, unknown_character_throws) {
  toml98::StupidLexer stupid("[[,]]");

  toml98::enter_array_key(stupid);

  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, eof_throws) {
  toml98::StupidLexer stupid("[[");

  toml98::enter_array_key(stupid);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}
