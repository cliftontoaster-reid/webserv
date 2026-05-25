#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>
#include <string>

#include "StupidLexer.hpp"

using namespace toml98;

static void expect_table_key_token(Token* token, TokenType expected_type) {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, expected_type);
  delete token;
}

Test(lexer_handle_table_key, simple_name) {
  StupidLexer stupid("table]");
  stupid.stupid_push_state(LexerTableKey);
  Token* token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenTableEnd);
  std::stack<LexerState> state = stupid.stupid_stack();
  cr_expect(state.empty());
}

Test(lexer_handle_table_key, with_hyphen) {
  StupidLexer stupid("my-table]");
  stupid.stupid_push_state(LexerTableKey);
  Token* token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenTableEnd);
  std::stack<LexerState> state = stupid.stupid_stack();
  cr_expect(state.empty());
}

Test(lexer_handle_table_key, with_underscore) {
  StupidLexer stupid("my_table]");
  stupid.stupid_push_state(LexerTableKey);
  Token* token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenTableEnd);
  std::stack<LexerState> state = stupid.stupid_stack();
  cr_expect(state.empty());
}

Test(lexer_handle_table_key, with_dot) {
  StupidLexer stupid("table.sub]");
  stupid.stupid_push_state(LexerTableKey);
  Token* token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenDot);
  delete token;
  token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenTableEnd);
  std::stack<LexerState> state = stupid.stupid_stack();
  cr_expect(state.empty());
}

Test(lexer_handle_table_key, numeric_suffix) {
  StupidLexer stupid("table1]");
  stupid.stupid_push_state(LexerTableKey);
  Token* token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenTableEnd);
  std::stack<LexerState> state = stupid.stupid_stack();
  cr_expect(state.empty());
}

Test(lexer_handle_table_key, dotted_path) {
  StupidLexer stupid("a.b.c]");
  stupid.stupid_push_state(LexerTableKey);
  Token* token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenDot);
  delete token;
  token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenDot);
  delete token;
  token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenTableEnd);
  std::stack<LexerState> state = stupid.stupid_stack();
  cr_expect(state.empty());
}

Test(lexer_handle_table_key, quoted_string_double) {
  StupidLexer stupid("\"quoted key\"]");
  stupid.stupid_push_state(LexerTableKey);
  Token* token = stupid.stupid_handle_table_key();
  cr_assert_not_null(token);
  cr_assert_eq(token->type, TokenString);
  delete token;
  std::stack<LexerState> state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), LexerTableKey);
}

Test(lexer_handle_table_key, quoted_string_single) {
  StupidLexer stupid("'quoted key']");
  stupid.stupid_push_state(LexerTableKey);
  Token* token = stupid.stupid_handle_table_key();
  cr_assert_not_null(token);
  cr_assert_eq(token->type, TokenString);
  delete token;
  std::stack<LexerState> state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), LexerTableKey);
}

Test(lexer_handle_table_key, mixed_quoted_unquoted) {
  StupidLexer stupid("unquoted.\"quoted\".unquoted]");
  stupid.stupid_push_state(LexerTableKey);
  Token* token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenDot);
  delete token;
  token = stupid.stupid_handle_table_key();
  cr_assert_not_null(token);
  cr_assert_eq(token->type, TokenString);
  delete token;
  token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenDot);
  delete token;
  token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenTableEnd);
  std::stack<LexerState> state = stupid.stupid_stack();
  cr_expect(state.empty());
}

Test(lexer_handle_table_key, with_whitespace_before_close) {
  StupidLexer stupid("table  ]");
  stupid.stupid_push_state(LexerTableKey);
  Token* token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenTableEnd);
  std::stack<LexerState> state = stupid.stupid_stack();
  cr_expect(state.empty());
}

Test(lexer_handle_table_key, numeric_only) {
  StupidLexer stupid("123]");
  stupid.stupid_push_state(LexerTableKey);
  Token* token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenTableEnd);
  std::stack<LexerState> state = stupid.stupid_stack();
  cr_expect(state.empty());
}

Test(lexer_handle_table_key, empty_brackets) {
  StupidLexer stupid("]");
  stupid.stupid_push_state(LexerTableKey);
  Token* token = stupid.stupid_handle_table_key();
  expect_table_key_token(token, TokenTableEnd);
  std::stack<LexerState> state = stupid.stupid_stack();
  cr_expect(state.empty());
}

Test(lexer_handle_table_key, unterminated_eof) {
  StupidLexer stupid("table");
  stupid.stupid_push_state(LexerTableKey);
  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, nested_table_bracket) {
  StupidLexer stupid("[table]");
  stupid.stupid_push_state(LexerTableKey);
  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, nested_array_bracket) {
  StupidLexer stupid("table[sub]]");
  stupid.stupid_push_state(LexerTableKey);
  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, triple_quote_double) {
  StupidLexer stupid("\"\"\"multiline\"\"\"]");
  stupid.stupid_push_state(LexerTableKey);
  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, triple_quote_single) {
  StupidLexer stupid("'''multiline''']");
  stupid.stupid_push_state(LexerTableKey);
  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, unterminated_double_quote) {
  StupidLexer stupid("\"unterminated");
  stupid.stupid_push_state(LexerTableKey);
  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, unterminated_single_quote) {
  StupidLexer stupid("'unterminated");
  stupid.stupid_push_state(LexerTableKey);
  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, unknown_character_at) {
  StupidLexer stupid("table@key]");
  stupid.stupid_push_state(LexerTableKey);
  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, unknown_character_hash) {
  StupidLexer stupid("table#key]");
  stupid.stupid_push_state(LexerTableKey);
  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, special_char_dollar) {
  StupidLexer stupid("table$key]");
  stupid.stupid_push_state(LexerTableKey);
  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, special_char_percent) {
  StupidLexer stupid("table%key]");
  stupid.stupid_push_state(LexerTableKey);
  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, eof_in_middle_of_quoted_string) {
  StupidLexer stupid("\"hello");
  stupid.stupid_push_state(LexerTableKey);
  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}
