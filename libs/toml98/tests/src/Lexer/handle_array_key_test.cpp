#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>
#include <string>

#include "StupidLexer.hpp"

using namespace toml98;

static void expect_array_key_token(Token* token, TokenType expected_type) {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, expected_type);
  delete token;
}

Test(lexer_handle_array_key, simple_name) {
  StupidLexer stupid("array]]");
  stupid.stupid_push_state(LexerArrayKey);
  Token* token = stupid.stupid_handle_array_key();
  expect_array_key_token(token, TokenArrayKeyEnd);
}

Test(lexer_handle_array_key, with_hyphen) {
  StupidLexer stupid("my-array]]");
  stupid.stupid_push_state(LexerArrayKey);
  Token* token = stupid.stupid_handle_array_key();
  expect_array_key_token(token, TokenArrayKeyEnd);
}

Test(lexer_handle_array_key, with_underscore) {
  StupidLexer stupid("my_array]]");
  stupid.stupid_push_state(LexerArrayKey);
  Token* token = stupid.stupid_handle_array_key();
  expect_array_key_token(token, TokenArrayKeyEnd);
}

Test(lexer_handle_array_key, with_dot) {
  StupidLexer stupid("array.sub]]");
  stupid.stupid_push_state(LexerArrayKey);
  Token* token = stupid.stupid_handle_array_key();
  expect_array_key_token(token, TokenDot);
  delete token;
}

Test(lexer_handle_array_key, numeric_suffix) {
  StupidLexer stupid("array1]]");
  stupid.stupid_push_state(LexerArrayKey);
  Token* token = stupid.stupid_handle_array_key();
  expect_array_key_token(token, TokenArrayKeyEnd);
}

Test(lexer_handle_array_key, dotted_path) {
  StupidLexer stupid("a.b.c]]");
  stupid.stupid_push_state(LexerArrayKey);
  Token* token = stupid.stupid_handle_array_key();
  expect_array_key_token(token, TokenDot);
  delete token;
}

Test(lexer_handle_array_key, quoted_string_double) {
  StupidLexer stupid("\"quoted key\"]]");
  stupid.stupid_push_state(LexerArrayKey);
  Token* token = stupid.stupid_handle_array_key();
  cr_assert_not_null(token);
  cr_expect_eq(token->type, TokenString);
  delete token;
}

Test(lexer_handle_array_key, quoted_string_single) {
  StupidLexer stupid("'quoted key']]");
  stupid.stupid_push_state(LexerArrayKey);
  Token* token = stupid.stupid_handle_array_key();
  cr_assert_not_null(token);
  cr_expect_eq(token->type, TokenString);
  delete token;
}

Test(lexer_handle_array_key, mixed_quoted_unquoted) {
  StupidLexer stupid("unquoted.\"quoted\".unquoted]]");
  stupid.stupid_push_state(LexerArrayKey);
  Token* token = stupid.stupid_handle_array_key();
  expect_array_key_token(token, TokenArrayKeyEnd);
}

Test(lexer_handle_array_key, with_whitespace_before_close) {
  StupidLexer stupid("array  ]]");
  stupid.stupid_push_state(LexerArrayKey);
  Token* token = stupid.stupid_handle_array_key();
  expect_array_key_token(token, TokenArrayKeyEnd);
}

Test(lexer_handle_array_key, numeric_only) {
  StupidLexer stupid("123]]");
  stupid.stupid_push_state(LexerArrayKey);
  Token* token = stupid.stupid_handle_array_key();
  expect_array_key_token(token, TokenArrayKeyEnd);
}

Test(lexer_handle_array_key, unterminated_eof) {
  StupidLexer stupid("array");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, unterminated_single_bracket) {
  StupidLexer stupid("array]");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, nested_array_bracket) {
  StupidLexer stupid("array[[sub]]]");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, nested_single_bracket) {
  StupidLexer stupid("array[sub]]]");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, triple_quote_double) {
  StupidLexer stupid("\"\"\"multiline\"\"\"]]");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, triple_quote_single) {
  StupidLexer stupid("'''multiline''']]");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, unterminated_double_quote) {
  StupidLexer stupid("\"unterminated]]");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, unterminated_single_quote) {
  StupidLexer stupid("'unterminated]]");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, single_bracket_in_middle) {
  StupidLexer stupid("table]middle]]");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, unknown_character_at) {
  StupidLexer stupid("array@key]]");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, special_char_hash) {
  StupidLexer stupid("array#key]]");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, empty_double_brackets) {
  StupidLexer stupid("]]");
  stupid.stupid_push_state(LexerArrayKey);
  Token* token = stupid.stupid_handle_array_key();
  expect_array_key_token(token, TokenArrayKeyEnd);
}

Test(lexer_handle_array_key, special_char_dollar) {
  StupidLexer stupid("array$key]]");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}

Test(lexer_handle_array_key, eof_in_middle_of_quoted) {
  StupidLexer stupid("\"hello");
  stupid.stupid_push_state(LexerArrayKey);
  cr_expect_throw(stupid.stupid_handle_array_key(), std::runtime_error);
}
