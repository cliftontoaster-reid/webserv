#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>
#include <string>

#include "StupidLexer.hpp"

using namespace toml98;

static void expect_word_token(Token* token, const std::string& expected) {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, TokenWord);
  cr_assert_str_eq(token->value.c_str(), expected.c_str());
  delete token;
}

// Normal Tests

Test(lexer_handle_word, simple_alphanumeric) {
  StupidLexer stupid("hello");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "hello");
  cr_assert_eq(stupid.stupid_pos(), 5);
}

Test(lexer_handle_word, with_hyphens) {
  StupidLexer stupid("my-key");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "my-key");
  cr_assert_eq(stupid.stupid_pos(), 6);
}

Test(lexer_handle_word, with_underscores) {
  StupidLexer stupid("my_key");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "my_key");
  cr_assert_eq(stupid.stupid_pos(), 6);
}

Test(lexer_handle_word, mixed_case) {
  StupidLexer stupid("MyKey123");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "MyKey123");
  cr_assert_eq(stupid.stupid_pos(), 8);
}

Test(lexer_handle_word, single_character) {
  StupidLexer stupid("a");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "a");
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_word, numeric_only) {
  StupidLexer stupid("123");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "123");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_word, hyphen_start) {
  StupidLexer stupid("-key");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "-key");
  cr_assert_eq(stupid.stupid_pos(), 4);
}

Test(lexer_handle_word, underscore_start) {
  StupidLexer stupid("_key");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "_key");
  cr_assert_eq(stupid.stupid_pos(), 4);
}

Test(lexer_handle_word, multiple_hyphens_underscores) {
  StupidLexer stupid("a-b_c-d_e");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "a-b_c-d_e");
  cr_assert_eq(stupid.stupid_pos(), 9);
}

// Boundary/Negative Tests

Test(lexer_handle_word, stops_at_space) {
  StupidLexer stupid("key value");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "key");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_word, stops_at_equals) {
  StupidLexer stupid("key=");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "key");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_word, stops_at_colon) {
  StupidLexer stupid("key:");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "key");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_word, stops_at_bracket) {
  StupidLexer stupid("key[");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "key");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_word, stops_at_brace) {
  StupidLexer stupid("key{");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "key");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_word, stops_at_comma) {
  StupidLexer stupid("key,");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "key");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_word, stops_at_dot) {
  StupidLexer stupid("key.sub");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "key");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_word, stops_at_hash) {
  StupidLexer stupid("key#comment");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "key");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_word, stops_at_quote) {
  StupidLexer stupid("key\"value");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "key");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_word, stops_at_newline) {
  StupidLexer stupid("key\n");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "key");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_word, stops_at_carriage_return) {
  StupidLexer stupid("key\r\n");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "key");
  cr_assert_eq(stupid.stupid_pos(), 3);
}

// Edge Cases

Test(lexer_handle_word, empty_source_returns_empty) {
  StupidLexer stupid("");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "");
  cr_assert_eq(stupid.stupid_pos(), 0);
}

Test(lexer_handle_word, at_eof_immediately) {
  StupidLexer stupid("\0");
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, "");
  cr_assert_eq(stupid.stupid_pos(), 0);
}

Test(lexer_handle_word, very_long_word) {
  std::string long_word(10000, 'a');
  StupidLexer stupid(long_word);
  Token* token = stupid.stupid_handle_word();
  expect_word_token(token, long_word);
  cr_assert_eq(stupid.stupid_pos(), 10000);
}
