#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <stack>
#include <stdexcept>
#include <string>
#include "StupidLexer.hpp"

using namespace toml98;

static void expect_multiline_token(Token* token, const std::string& expected) {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, TokenString);
  cr_assert_eq(token->value, expected);
  delete token;
}

Test(lexer_handle_string_double_multiline, simple_text) {
  StupidLexer stupid("hello\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "hello");
}

Test(lexer_handle_string_double_multiline, with_newlines) {
  StupidLexer stupid("hello\nworld\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "hello\nworld");
}

Test(lexer_handle_string_double_multiline, initial_newline_skip) {
  StupidLexer stupid("\nhello\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "hello");
}

Test(lexer_handle_string_double_multiline, initial_crlf_skip) {
  StupidLexer stupid("\r\nhello\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "hello");
}

Test(lexer_handle_string_double_multiline, escape_newline) {
  StupidLexer stupid("hello\\nworld\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "hello\nworld");
}

Test(lexer_handle_string_double_multiline, escape_tab) {
  StupidLexer stupid("hello\\tworld\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "hello\tworld");
}

Test(lexer_handle_string_double_multiline, escape_backslash) {
  StupidLexer stupid("path\\\\to\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "path\\to");
}

Test(lexer_handle_string_double_multiline, embedded_double_quotes) {
  StupidLexer stupid("he said \"hi\" \"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "he said \"hi\" ");
}

Test(lexer_handle_string_double_multiline, empty) {
  StupidLexer stupid("\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "");
}

Test(lexer_handle_string_double_multiline, mixed_escapes) {
  StupidLexer stupid("a\\nb\\tc\\n\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "a\nb\tc\n");
}

Test(lexer_handle_string_double_multiline, unicode_escape_u) {
  StupidLexer stupid("hello\\u0041\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "helloA");
}

Test(lexer_handle_string_double_multiline, unicode_escape_x) {
  StupidLexer stupid("hello\\x42\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "helloB");
}

Test(lexer_handle_string_double_multiline, four_quotes_terminates) {
  StupidLexer stupid("content\"\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "content\"");
}

Test(lexer_handle_string_double_multiline, initial_cr_only_skip) {
  StupidLexer stupid("\rx\"\"\"");
  Token* token = stupid.stupid_handle_string_double_multiline();
  expect_multiline_token(token, "x");
}

Test(lexer_handle_string_double_multiline, unterminated_at_eof) {
  StupidLexer stupid("hello");
  cr_expect_throw(stupid.stupid_handle_string_double_multiline(), std::runtime_error);
}

Test(lexer_handle_string_double_multiline, only_two_quotes_close) {
  StupidLexer stupid("value\"\"");
  cr_expect_throw(stupid.stupid_handle_string_double_multiline(), std::runtime_error);
}

Test(lexer_handle_string_double_multiline, only_one_quote_close) {
  StupidLexer stupid("value\"");
  cr_expect_throw(stupid.stupid_handle_string_double_multiline(), std::runtime_error);
}

Test(lexer_handle_string_double_multiline, invalid_escape_sequence) {
  StupidLexer stupid("\\q\"\"\"");
  cr_expect_throw(stupid.stupid_handle_string_double_multiline(), std::runtime_error);
}

Test(lexer_handle_string_double_multiline, escape_at_eof) {
  StupidLexer stupid("hello\\");
  cr_expect_throw(stupid.stupid_handle_string_double_multiline(), std::runtime_error);
}

Test(lexer_handle_string_double_multiline, incomplete_unicode_x) {
  StupidLexer stupid("\\xH\"\"\"");
  cr_expect_throw(stupid.stupid_handle_string_double_multiline(), std::runtime_error);
}

Test(lexer_handle_string_double_multiline, incomplete_unicode_u) {
  StupidLexer stupid("\\uHH\"\"\"");
  cr_expect_throw(stupid.stupid_handle_string_double_multiline(), std::runtime_error);
}

Test(lexer_handle_string_double_multiline, invalid_hex_digit) {
  StupidLexer stupid("\\xZZ\"\"\"");
  cr_expect_throw(stupid.stupid_handle_string_double_multiline(), std::runtime_error);
}

Test(lexer_handle_string_double_multiline, very_long_unterminated) {
  StupidLexer stupid(std::string(10000, 'a'));
  cr_expect_throw(stupid.stupid_handle_string_double_multiline(), std::runtime_error);
}

Test(lexer_handle_string_double_multiline, backslash_replaces_closing) {
  StupidLexer stupid("...\\\"\"\"");
  cr_expect_throw(stupid.stupid_handle_string_double_multiline(), std::runtime_error);
}

Test(lexer_handle_string_double_multiline, empty_source_unterminated) {
  StupidLexer stupid("");
  cr_expect_throw(stupid.stupid_handle_string_double_multiline(), std::runtime_error);
}

Test(lexer_handle_string_double_multiline, incomplete_U_escape) {
  StupidLexer stupid("\\U000000\"\"\"");
  cr_expect_throw(stupid.stupid_handle_string_double_multiline(), std::runtime_error);
}
