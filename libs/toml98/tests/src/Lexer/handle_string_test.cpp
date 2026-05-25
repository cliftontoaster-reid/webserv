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

// Normal tests

Test(lexer_handle_string, simple_text) {
  StupidLexer stupid("hello']");
  stupid.stupid_stack().push(LexerString);

  Token* token = stupid.stupid_handle_string();
  expect_string_token(token, "hello");
}

Test(lexer_handle_string, with_spaces) {
  StupidLexer stupid("hello world'");
  stupid.stupid_stack().push(LexerString);

  Token* token = stupid.stupid_handle_string();
  expect_string_token(token, "hello world");
}

Test(lexer_handle_string, with_special_chars) {
  StupidLexer stupid("hello@#$%^&*()'");
  stupid.stupid_stack().push(LexerString);

  Token* token = stupid.stupid_handle_string();
  expect_string_token(token, "hello@#$%^&*()");
}

Test(lexer_handle_string, with_backslashes) {
  StupidLexer stupid("path\\to\\file'");
  stupid.stupid_stack().push(LexerString);

  Token* token = stupid.stupid_handle_string();
  expect_string_token(token, "path\\to\\file");
}

Test(lexer_handle_string, empty) {
  StupidLexer stupid("'");
  stupid.stupid_stack().push(LexerString);

  Token* token = stupid.stupid_handle_string();
  expect_string_token(token, "");
}

Test(lexer_handle_string, with_double_quotes) {
  StupidLexer stupid("he said \"hello\"'");
  stupid.stupid_stack().push(LexerString);

  Token* token = stupid.stupid_handle_string();
  expect_string_token(token, "he said \"hello\"");
}

Test(lexer_handle_string, with_unicode) {
  StupidLexer stupid("héllo wörld'");
  stupid.stupid_stack().push(LexerString);

  Token* token = stupid.stupid_handle_string();
  expect_string_token(token, "héllo wörld");
}

Test(lexer_handle_string, with_tab) {
  StupidLexer stupid("hello\tworld'");
  stupid.stupid_stack().push(LexerString);

  Token* token = stupid.stupid_handle_string();
  expect_string_token(token, "hello\tworld");
}

Test(lexer_handle_string, with_nested_single_quotes) {
  StupidLexer stupid("it's fine'");
  stupid.stupid_stack().push(LexerString);

  Token* token = stupid.stupid_handle_string();
  expect_string_token(token, "it's fine");
}

// Negative tests

Test(lexer_handle_string, unterminated_at_eof) {
  StupidLexer stupid("hello");
  stupid.stupid_stack().push(LexerString);

  cr_expect_throw(stupid.stupid_handle_string(), std::runtime_error);
}

Test(lexer_handle_string, newline_unix_embedded) {
  StupidLexer stupid("hello\nworld'");
  stupid.stupid_stack().push(LexerString);

  cr_expect_throw(stupid.stupid_handle_string(), std::runtime_error);
}

Test(lexer_handle_string, newline_windows_embedded) {
  StupidLexer stupid("hello\r\nworld'");
  stupid.stupid_stack().push(LexerString);

  cr_expect_throw(stupid.stupid_handle_string(), std::runtime_error);
}

Test(lexer_handle_string, carriage_return_embedded) {
  StupidLexer stupid("hello\rworld'");
  stupid.stupid_stack().push(LexerString);

  cr_expect_throw(stupid.stupid_handle_string(), std::runtime_error);
}

Test(lexer_handle_string, newline_then_eof) {
  StupidLexer stupid("hello\n");
  stupid.stupid_stack().push(LexerString);

  cr_expect_throw(stupid.stupid_handle_string(), std::runtime_error);
}

Test(lexer_handle_string, multiple_newlines) {
  StupidLexer stupid("hello\nworld\nfoo'");
  stupid.stupid_stack().push(LexerString);

  cr_expect_throw(stupid.stupid_handle_string(), std::runtime_error);
}

Test(lexer_handle_string, eof_immediately_after_open) {
  StupidLexer stupid("");
  stupid.stupid_stack().push(LexerString);

  cr_expect_throw(stupid.stupid_handle_string(), std::runtime_error);
}

Test(lexer_handle_string, very_long_unterminated) {
  std::string longStr(10000, 'a');
  StupidLexer stupid(longStr);
  stupid.stupid_stack().push(LexerString);

  cr_expect_throw(stupid.stupid_handle_string(), std::runtime_error);
}
