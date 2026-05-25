#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>
#include <string>

#include "StupidLexer.hpp"

namespace toml98 {

static void expect_word_token(Token* token, const std::string& expected) {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, TokenWord);
  cr_assert_eq(token->value, expected);
  delete token;
}

}  // namespace toml98

Test(lexer_handle_word, alpha) {
  toml98::StupidLexer stupid("a");

  toml98::Token* token = stupid.stupid_handle_word();
  toml98::expect_word_token(token, "a");
  cr_assert_eq(stupid.stupid_pos(), 1u);
}

Test(lexer_handle_word, numeric) {
  toml98::StupidLexer stupid("7");

  toml98::Token* token = stupid.stupid_handle_word();
  toml98::expect_word_token(token, "7");
  cr_assert_eq(stupid.stupid_pos(), 1u);
}

Test(lexer_handle_word, mixed_alnum) {
  toml98::StupidLexer stupid("abc123");

  toml98::Token* token = stupid.stupid_handle_word();
  toml98::expect_word_token(token, "abc123");
  cr_assert_eq(stupid.stupid_pos(), 6u);
}

Test(lexer_handle_word, hyphen) {
  toml98::StupidLexer stupid("a-b");

  toml98::Token* token = stupid.stupid_handle_word();
  toml98::expect_word_token(token, "a-b");
  cr_assert_eq(stupid.stupid_pos(), 3u);
}

Test(lexer_handle_word, underscore) {
  toml98::StupidLexer stupid("a_b");

  toml98::Token* token = stupid.stupid_handle_word();
  toml98::expect_word_token(token, "a_b");
  cr_assert_eq(stupid.stupid_pos(), 3u);
}

Test(lexer_handle_word, stops_before_space) {
  toml98::StupidLexer stupid("abc def");

  toml98::Token* token = stupid.stupid_handle_word();
  toml98::expect_word_token(token, "abc");
  cr_assert_eq(stupid.stupid_pos(), 3u);
  cr_assert_eq(stupid.stupid_peek(), ' ');
}

Test(lexer_handle_word, stops_before_newline) {
  toml98::StupidLexer stupid("abc\n");

  toml98::Token* token = stupid.stupid_handle_word();
  toml98::expect_word_token(token, "abc");
  cr_assert_eq(stupid.stupid_pos(), 3u);
  cr_assert_eq(stupid.stupid_peek(), '\n');
}

Test(lexer_handle_word, stops_before_comma) {
  toml98::StupidLexer stupid("abc,");

  toml98::Token* token = stupid.stupid_handle_word();
  toml98::expect_word_token(token, "abc");
  cr_assert_eq(stupid.stupid_pos(), 3u);
  cr_assert_eq(stupid.stupid_peek(), ',');
}

Test(lexer_handle_word, empty_input_throws) {
  toml98::StupidLexer stupid("");

  cr_assert_throw(stupid.stupid_handle_word(), std::runtime_error);
}

Test(lexer_handle_word, punctuation_only_throws) {
  toml98::StupidLexer stupid("!");

  cr_assert_throw(stupid.stupid_handle_word(), std::runtime_error);
}

Test(lexer_handle_word, normal_alpha) {
  toml98::StupidLexer stupid("a");
  stupid.stupid_handle_normal();

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}
