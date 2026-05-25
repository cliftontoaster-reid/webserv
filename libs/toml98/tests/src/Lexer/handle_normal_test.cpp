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
    cr_assert_eq(token->value, expected_value);
  }
  delete token;
}

// Helper function to verify state stack after handle_normal
static void verify_state_pushed(std::stack<LexerState> state,
                                LexerState expected_top) {
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected_top);
}

// ============================================================================
// NORMAL TESTS (17 tests)
// ============================================================================

Test(lexer_handle_normal, comment_transition) {
  StupidLexer stupid("# comment");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerComments);
}

Test(lexer_handle_normal, space_transition) {
  StupidLexer stupid(" ");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerWhiteSpace);
}

Test(lexer_handle_normal, tab_transition) {
  StupidLexer stupid("\t");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerWhiteSpace);
}

Test(lexer_handle_normal, single_quote_string) {
  StupidLexer stupid("'hello'");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerString);
}

Test(lexer_handle_normal, double_quote_string) {
  StupidLexer stupid("\"hello\"");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerStringDouble);
}

Test(lexer_handle_normal, left_brace_token) {
  StupidLexer stupid("{");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  expect_token(token, TokenTableStart, "{");
}

Test(lexer_handle_normal, dot_token) {
  StupidLexer stupid(".");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  expect_token(token, TokenDot, "");
}

Test(lexer_handle_normal, newline_unix) {
  StupidLexer stupid("\n");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  expect_token(token, TokenNewLine, "");
}

Test(lexer_handle_normal, newline_windows) {
  StupidLexer stupid("\r\n");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  expect_token(token, TokenNewLine, "");
}

Test(lexer_handle_normal, word_start_hyphen) {
  StupidLexer stupid("-key");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerWord);
}

Test(lexer_handle_normal, word_start_underscore) {
  StupidLexer stupid("_key");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerWord);
}

Test(lexer_handle_normal, word_start_alphanumeric) {
  StupidLexer stupid("key");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerWord);
}

Test(lexer_handle_normal, triple_single_quote_multiline) {
  StupidLexer stupid("'''\nmultiline'''");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerStringMultiLine);
}

Test(lexer_handle_normal, triple_double_quote_multiline) {
  StupidLexer stupid("\"\"\"\nmultiline\"\"\"");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerStringDoubleMultiLine);
}

Test(lexer_handle_normal, single_bracket_table_key) {
  StupidLexer stupid("[table]");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  expect_token(token, TokenTableKeyStart, "");
}

Test(lexer_handle_normal, double_bracket_array_key) {
  StupidLexer stupid("[[array]]");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  expect_token(token, TokenArrayStart, "[[");
}

Test(lexer_handle_normal, single_bracket_inline_array) {
  StupidLexer stupid("[1, 2]");
  stupid.stupid_is_last_equal() = true;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  expect_token(token, TokenArrayStart, "[");
}

// ============================================================================
// NEGATIVE TESTS (15 tests)
// ============================================================================

Test(lexer_handle_normal, unterminated_string_single_quote_eof) {
  StupidLexer stupid("'");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}

Test(lexer_handle_normal, unterminated_string_double_quote_eof) {
  StupidLexer stupid("\"");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}

Test(lexer_handle_normal, invalid_carriage_return_alone) {
  StupidLexer stupid("\r");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}

Test(lexer_handle_normal, carriage_return_not_followed_by_newline) {
  StupidLexer stupid("\rx");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}

Test(lexer_handle_normal, unknown_character_at_sign) {
  StupidLexer stupid("@");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}

Test(lexer_handle_normal, unknown_character_exclamation) {
  StupidLexer stupid("!");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}

Test(lexer_handle_normal, unknown_character_dollar) {
  StupidLexer stupid("$");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}

Test(lexer_handle_normal, unknown_character_percent) {
  StupidLexer stupid("%");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}

Test(lexer_handle_normal, unknown_character_ampersand) {
  StupidLexer stupid("&");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}

Test(lexer_handle_normal, unknown_character_caret) {
  StupidLexer stupid("^");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}

Test(lexer_handle_normal, unknown_character_pipe) {
  StupidLexer stupid("|");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}

Test(lexer_handle_normal, incomplete_triple_quote_second_missing) {
  StupidLexer stupid("''x");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);
  std::stack<LexerState> state = stupid.stupid_stack();
  verify_state_pushed(state, LexerString);
}

Test(lexer_handle_normal, control_character_bell) {
  StupidLexer stupid("\x07");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}

Test(lexer_handle_normal, eof_immediately) {
  StupidLexer stupid("");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  Token* token = stupid.stupid_handle_normal();
  cr_expect_null(token);
}

Test(lexer_handle_normal, null_character) {
  StupidLexer stupid("\0");
  stupid.stupid_is_last_equal() = false;
  stupid.stupid_push_state(LexerNormal);
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}
