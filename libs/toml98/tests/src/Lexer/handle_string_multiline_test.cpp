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

Test(lexer_handle_string_multiline, simple_text) {
  StupidLexer stupid("hello'''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "hello");
}

Test(lexer_handle_string_multiline, with_newlines) {
  StupidLexer stupid("hello\nworld'''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "hello\nworld");
}

Test(lexer_handle_string_multiline, initial_newline_skip) {
  StupidLexer stupid("\nhello'''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "hello");
}

Test(lexer_handle_string_multiline, initial_crlf_skip) {
  StupidLexer stupid("\r\nhello'''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "hello");
}

Test(lexer_handle_string_multiline, embedded_double_quotes) {
  StupidLexer stupid("say \"hello\"'''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "say \"hello\"");
}

Test(lexer_handle_string_multiline, embedded_single_quote) {
  StupidLexer stupid("it's fine'''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "it's fine");
}

Test(lexer_handle_string_multiline, embedded_two_quotes) {
  StupidLexer stupid("test''value'''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "test''value");
}

Test(lexer_handle_string_multiline, empty) {
  StupidLexer stupid("'''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "");
}

Test(lexer_handle_string_multiline, tabs_and_spaces) {
  StupidLexer stupid("  hello\tworld  '''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "  hello\tworld  ");
}

Test(lexer_handle_string_multiline, with_backslashes) {
  StupidLexer stupid("path\\to\\file'''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "path\\to\\file");
}

Test(lexer_handle_string_multiline, four_quotes_terminates) {
  StupidLexer stupid("content''''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "content'");
}

Test(lexer_handle_string_multiline, five_quotes_terminates) {
  StupidLexer stupid("content'''''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "content''");
}

Test(lexer_handle_string_multiline, multiple_lines_with_trailing) {
  StupidLexer stupid("line1\nline2\nline3'''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "line1\nline2\nline3");
}

Test(lexer_handle_string_multiline, initial_cr_only_skip) {
  StupidLexer stupid("\rx'''");
  Token* token = stupid.stupid_handle_string_multiline();
  expect_multiline_token(token, "x");
}

Test(lexer_handle_string_multiline, unterminated_at_eof) {
  StupidLexer stupid("hello");
  cr_expect_throw(stupid.stupid_handle_string_multiline(), std::runtime_error);
}

Test(lexer_handle_string_multiline, only_two_quotes_close) {
  StupidLexer stupid("value''");
  cr_expect_throw(stupid.stupid_handle_string_multiline(), std::runtime_error);
}

Test(lexer_handle_string_multiline, only_one_quote_close) {
  StupidLexer stupid("value'");
  cr_expect_throw(stupid.stupid_handle_string_multiline(), std::runtime_error);
}

Test(lexer_handle_string_multiline, very_long_unterminated) {
  std::string long_input(10000, 'a');
  StupidLexer stupid(long_input);
  cr_expect_throw(stupid.stupid_handle_string_multiline(), std::runtime_error);
}

Test(lexer_handle_string_multiline, no_content_unterminated) {
  StupidLexer stupid("");
  cr_expect_throw(stupid.stupid_handle_string_multiline(), std::runtime_error);
}

Test(lexer_handle_string_multiline, unterminated_with_embedded_quotes) {
  StupidLexer stupid("a''b'c''");
  cr_expect_throw(stupid.stupid_handle_string_multiline(), std::runtime_error);
}
