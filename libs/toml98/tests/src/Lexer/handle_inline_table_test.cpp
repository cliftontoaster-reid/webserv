#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>
#include <string>

#include "StupidLexer.hpp"

using namespace toml98;

// Helper function to verify TokenTableEnd token
static void expect_table_end_token(Token* token) {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, TokenTableEnd);
  delete token;
}

// Helper function to verify generic token type and value
static void expect_token(Token* token, TokenType expected_type,
                         const std::string& expected_value = "") {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, expected_type);
  if (!expected_value.empty()) {
    cr_assert_str_eq(token->value.c_str(), expected_value.c_str());
  }
  delete token;
}

// Helper function to verify state stack
static void verify_state_on_stack(std::stack<LexerState> state,
                                  LexerState expected_state) {
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected_state);
}

// ============================================================================
// NORMAL TESTS (13 tests)
// ============================================================================

Test(lexer_handle_inline_table, closing_brace) {
  StupidLexer stupid("}");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  expect_table_end_token(token);
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_inline_table, with_whitespace) {
  StupidLexer stupid(" }");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_on_stack(state, LexerWhiteSpace);
}

Test(lexer_handle_inline_table, with_comment) {
  StupidLexer stupid(" # comment\n}");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_on_stack(state, LexerWhiteSpace);
}

Test(lexer_handle_inline_table, simple_key_value) {
  StupidLexer stupid("key=\"value\"}");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_on_stack(state, LexerWord);
}

Test(lexer_handle_inline_table, colon_as_equals) {
  StupidLexer stupid("key:\"value\"}");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_on_stack(state, LexerWord);
}

Test(lexer_handle_inline_table, dot_key) {
  StupidLexer stupid("owner.name=\"John\"}");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_on_stack(state, LexerWord);
}

Test(lexer_handle_inline_table, nested_table) {
  StupidLexer stupid("{}");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  expect_token(token, TokenTableStart, "{");
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_on_stack(state, LexerInlineTable);
}

Test(lexer_handle_inline_table, multiline_string_value) {
  StupidLexer stupid("key=\"\"\"multiline\"\"\"}");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_on_stack(state, LexerWord);
}

Test(lexer_handle_inline_table, inline_array_value) {
  StupidLexer stupid("key=[1,2,3]}");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_on_stack(state, LexerWord);
}

Test(lexer_handle_inline_table, double_quoted_key) {
  StupidLexer stupid("\"quoted key\"=\"value\"}");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_on_stack(state, LexerStringDouble);
}

Test(lexer_handle_inline_table, single_quoted_key) {
  StupidLexer stupid("'quoted key'=\"value\"}");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_on_stack(state, LexerString);
}

Test(lexer_handle_inline_table, with_newlines) {
  StupidLexer stupid("\nkey=\"val\"\n}");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  expect_token(token, TokenNewLine);
}

Test(lexer_handle_inline_table, multiple_key_value_pairs) {
  StupidLexer stupid("a=1\nb=\"two\"\nc=true }");
  stupid.stupid_push_state(LexerInlineTable);

  Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_on_stack(state, LexerWord);
}

// ============================================================================
// NEGATIVE TESTS (13 tests - should throw)
// ============================================================================

Test(lexer_handle_inline_table, unterminated_string_double) {
  StupidLexer stupid("key=\"unclosed");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, unterminated_string_single) {
  StupidLexer stupid("key='unclosed");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, invalid_carriage_return) {
  StupidLexer stupid("\r");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, unknown_character_at) {
  StupidLexer stupid("@");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, unknown_character_exclamation) {
  StupidLexer stupid("!");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, unknown_character_pipe) {
  StupidLexer stupid("|");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, unknown_character_question) {
  StupidLexer stupid("?");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, unterminated_eof) {
  StupidLexer stupid("key=\"value\"");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, control_character) {
  StupidLexer stupid("\x01");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, incomplete_string_at_eof) {
  StupidLexer stupid("key=\"hello");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, invalid_array_key_inside) {
  StupidLexer stupid("[[key]]");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, unknown_char_dollar) {
  StupidLexer stupid("$");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, very_long_unterminated) {
  StupidLexer stupid("key=\"val\",");
  stupid.stupid_push_state(LexerInlineTable);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}
