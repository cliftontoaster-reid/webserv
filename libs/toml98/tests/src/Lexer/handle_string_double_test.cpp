#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <stack>
#include <stdexcept>
#include <string>
#include "StupidLexer.hpp"

using namespace toml98;

static void expect_string_token(Token* token, const std::string& expected) {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, TokenString);
  cr_assert_eq(token->value, expected);
  delete token;
}

Test(lexer_handle_string_double, simple_text) {
  StupidLexer stupid("hello\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "hello");
}

Test(lexer_handle_string_double, with_spaces) {
  StupidLexer stupid("hello world\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "hello world");
}

Test(lexer_handle_string_double, escape_backslash) {
  StupidLexer stupid("\\\\\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "\\");
}

Test(lexer_handle_string_double, escape_quote) {
  StupidLexer stupid("\\\"\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "\"");
}

Test(lexer_handle_string_double, escape_tab) {
  StupidLexer stupid("\\t\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "\t");
}

Test(lexer_handle_string_double, escape_newline) {
  StupidLexer stupid("\\n\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "\n");
}

Test(lexer_handle_string_double, escape_backspace) {
  StupidLexer stupid("\\b\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "\b");
}

Test(lexer_handle_string_double, escape_formfeed) {
  StupidLexer stupid("\\f\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "\f");
}

Test(lexer_handle_string_double, escape_carriage_return) {
  StupidLexer stupid("\\r\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "\r");
}

Test(lexer_handle_string_double, escape_unicode_x) {
  StupidLexer stupid("\\x41\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "A");
}

Test(lexer_handle_string_double, escape_unicode_u) {
  StupidLexer stupid("\\u0041\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "A");
}

Test(lexer_handle_string_double, escape_unicode_U) {
  StupidLexer stupid("\\U00000041\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "A");
}

Test(lexer_handle_string_double, mixed_escapes) {
  StupidLexer stupid("hello\\nworld\\ttest\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "hello\nworld\ttest");
}

Test(lexer_handle_string_double, empty) {
  StupidLexer stupid("\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "");
}

Test(lexer_handle_string_double, escape_e) {
  StupidLexer stupid("\\e\"");
  Token* token = stupid.stupid_handle_string_double();
  expect_string_token(token, "\e");
}

Test(lexer_handle_string_double, unterminated_at_eof) {
  StupidLexer stupid("hello");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, newline_unix_embedded) {
  StupidLexer stupid("hello\nworld\"");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, newline_windows_embedded) {
  StupidLexer stupid("hello\r\nworld\"");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, carriage_return_embedded) {
  StupidLexer stupid("hello\rworld\"");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, invalid_escape_at_eof) {
  StupidLexer stupid("test\\");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, invalid_escape_sequence) {
  StupidLexer stupid("\\q\"");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, invalid_escape_unknown) {
  StupidLexer stupid("\\?\"");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, incomplete_x_escape_no_digits) {
  StupidLexer stupid("\\x\"");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, incomplete_x_escape_one_digit) {
  StupidLexer stupid("\\xA\"");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, incomplete_u_escape_no_digits) {
  StupidLexer stupid("\\u\"");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, incomplete_U_escape_no_digits) {
  StupidLexer stupid("\\U\"");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, invalid_hex_digit_x) {
  StupidLexer stupid("\\xZZ\"");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, invalid_hex_digit_u) {
  StupidLexer stupid("\\uGGGG\"");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, unterminated_after_escape) {
  StupidLexer stupid("\\n");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, eof_after_backslash) {
  StupidLexer stupid("test\\");
  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}
