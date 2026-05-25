#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>
#include <string>

#include "StupidLexer.hpp"

using namespace toml98;

// Helper function to verify token type and content
static void expect_token(Token* token, TokenType expected_type,
                         const std::string& expected_value = "") {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, expected_type);
  if (!expected_value.empty()) {
    cr_assert_str_eq(token->value.c_str(), expected_value.c_str());
  }
  delete token;
}

// Helper function to verify state stack after state transition
static void verify_state_pushed(std::stack<LexerState> state,
                                LexerState expected_top) {
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected_top);
}

// ============================================================================
// NORMAL TESTS (13 tests)
// ============================================================================

Test(lexer_handle_inline_array, closing_bracket) {
  StupidLexer stupid("]");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  expect_token(token, TokenArrayEnd, "]");
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_inline_array, with_whitespace) {
  StupidLexer stupid(" ]");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerWhiteSpace);
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_inline_array, with_comment) {
  StupidLexer stupid("# comment\n]");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerComments);
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_inline_array, with_newlines) {
  StupidLexer stupid("\n]");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  expect_token(token, TokenNewLine, "\n");
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_inline_array, nested_array) {
  StupidLexer stupid("[]");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  expect_token(token, TokenArrayStart, "[");
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerInlineArray);
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_inline_array, string_double) {
  StupidLexer stupid("\"value\"]");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerStringDouble);
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_inline_array, string_single) {
  StupidLexer stupid("'value']");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerString);
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_inline_array, multiline_string_double) {
  StupidLexer stupid("\"\"\"value\"\"\" ]");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerStringDoubleMultiLine);
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_inline_array, multiline_string_single) {
  StupidLexer stupid("'''value''']");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerStringMultiLine);
  cr_assert_eq(stupid.stupid_pos(), 3);
}

Test(lexer_handle_inline_array, inline_table) {
  StupidLexer stupid("{key=\"val\"}]");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  expect_token(token, TokenTableStart, "{");
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerInlineTable);
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_inline_array, word_tokens) {
  StupidLexer stupid("key, value]");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerWord);
  cr_assert_eq(stupid.stupid_pos(), 0);
}

Test(lexer_handle_inline_array, dot_token) {
  StupidLexer stupid(".sub]");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  expect_token(token, TokenDot, ".");
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_inline_array, double_bracket_array_key) {
  StupidLexer stupid("[[nested]]]");
  stupid.stupid_push_state(LexerInlineArray);
  Token* token = stupid.stupid_handle_inline_array();
  expect_token(token, TokenArrayStart, "[[");
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerArrayKey);
  cr_assert_eq(stupid.stupid_pos(), 2);
}

// ============================================================================
// NEGATIVE TESTS (12 tests)
// ============================================================================

Test(lexer_handle_inline_array, unterminated_string_double) {
  StupidLexer stupid("\"unclosed");
  stupid.stupid_push_state(LexerInlineArray);
  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, unterminated_string_single) {
  StupidLexer stupid("'unclosed");
  stupid.stupid_push_state(LexerInlineArray);
  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, invalid_carriage_return) {
  StupidLexer stupid("\r");
  stupid.stupid_push_state(LexerInlineArray);
  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, unknown_character_at) {
  StupidLexer stupid("@");
  stupid.stupid_push_state(LexerInlineArray);
  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, unknown_character_exclamation) {
  StupidLexer stupid("!");
  stupid.stupid_push_state(LexerInlineArray);
  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, unknown_character_pipe) {
  StupidLexer stupid("|");
  stupid.stupid_push_state(LexerInlineArray);
  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, unknown_character_question) {
  StupidLexer stupid("?");
  stupid.stupid_push_state(LexerInlineArray);
  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, unterminated_eof) {
  StupidLexer stupid("a, b, c");
  stupid.stupid_push_state(LexerInlineArray);
  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, control_character) {
  StupidLexer stupid("\x01");
  stupid.stupid_push_state(LexerInlineArray);
  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, incomplete_string_at_eof) {
  StupidLexer stupid("\"hello");
  stupid.stupid_push_state(LexerInlineArray);
  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, invalid_escape_in_string) {
  StupidLexer stupid("\"\\q\"");
  stupid.stupid_push_state(LexerInlineArray);
  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, very_long_unterminated_array) {
  StupidLexer stupid(
      "a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, "
      "y, z");
  stupid.stupid_push_state(LexerInlineArray);
  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}
