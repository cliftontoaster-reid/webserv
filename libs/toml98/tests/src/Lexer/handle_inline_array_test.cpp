#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>
#include <string>

#include "StupidLexer.hpp"

namespace toml98 {

static void expect_token(Token* token, TokenType type,
                         const std::string& value) {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, type);
  cr_assert_eq(token->value, value);
  delete token;
}

static void enter_inline_array(StupidLexer& stupid) {
  stupid.stupid_stack().push(LexerInlineArray);
}

}  // namespace toml98

Test(lexer_handle_inline_array, comment_pushes_state) {
  toml98::StupidLexer stupid("#");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerComments);
}

Test(lexer_handle_inline_array, whitespace_pushes_state) {
  toml98::StupidLexer stupid(" ");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWhiteSpace);
}

Test(lexer_handle_inline_array, tab_pushes_state) {
  toml98::StupidLexer stupid("\t");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWhiteSpace);
}

Test(lexer_handle_inline_array, double_quote_string_state) {
  toml98::StupidLexer stupid("\"a");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerStringDouble);
}

Test(lexer_handle_inline_array, double_quote_multiline_string_state) {
  toml98::StupidLexer stupid("\"\"\"");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 3U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerStringDoubleMultiLine);
}

Test(lexer_handle_inline_array, single_quote_string_state) {
  toml98::StupidLexer stupid("'a");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerString);
}

Test(lexer_handle_inline_array, single_quote_multiline_string_state) {
  toml98::StupidLexer stupid("'''");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 3U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerStringMultiLine);
}

Test(lexer_handle_inline_array, nested_inline_table_token) {
  toml98::StupidLexer stupid("{");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  toml98::expect_token(token, toml98::TokenTableStart, "{");
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerInlineTable);
}

Test(lexer_handle_inline_array, nested_inline_array_token) {
  toml98::StupidLexer stupid("[");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  toml98::expect_token(token, toml98::TokenArrayStart, "[");
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerInlineArray);
}

Test(lexer_handle_inline_array, array_key_token) {
  toml98::StupidLexer stupid("[[");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  toml98::expect_token(token, toml98::TokenArrayStart, "[[");
  cr_expect_eq(stupid.stupid_pos(), 2U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerArrayKey);
}

Test(lexer_handle_inline_array, right_bracket_ends_inline_array) {
  toml98::StupidLexer stupid("]");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  toml98::expect_token(token, toml98::TokenArrayEnd, "]");
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
}

Test(lexer_handle_inline_array, equal_token) {
  toml98::StupidLexer stupid("=");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  toml98::expect_token(token, toml98::TokenEqual, "=");
  cr_expect_eq(stupid.stupid_pos(), 1U);
}

Test(lexer_handle_inline_array, word_state_from_alnum) {
  toml98::StupidLexer stupid("7");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}

Test(lexer_handle_inline_array, word_state_from_hyphen) {
  toml98::StupidLexer stupid("-");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}

Test(lexer_handle_inline_array, word_state_from_underscore) {
  toml98::StupidLexer stupid("_");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}

Test(lexer_handle_inline_array, dot_token) {
  toml98::StupidLexer stupid(".");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  toml98::expect_token(token, toml98::TokenDot, ".");
  cr_expect_eq(stupid.stupid_pos(), 1U);
}

Test(lexer_handle_inline_array, newline_token) {
  toml98::StupidLexer stupid("\n");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  toml98::expect_token(token, toml98::TokenNewLine, "\n");
  cr_expect_eq(stupid.stupid_pos(), 1U);
}

Test(lexer_handle_inline_array, crlf_token) {
  toml98::StupidLexer stupid("\r\n");
  toml98::enter_inline_array(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_array();
  toml98::expect_token(token, toml98::TokenNewLine, "\r\n");
  cr_expect_eq(stupid.stupid_pos(), 2U);
}

Test(lexer_handle_inline_array, carriage_return_throws) {
  toml98::StupidLexer stupid("\r");
  toml98::enter_inline_array(stupid);

  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, unknown_character_throws) {
  toml98::StupidLexer stupid(",");
  toml98::enter_inline_array(stupid);

  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, unterminated_double_quote_throws) {
  toml98::StupidLexer stupid("\"");
  toml98::enter_inline_array(stupid);

  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}

Test(lexer_handle_inline_array, unterminated_single_quote_throws) {
  toml98::StupidLexer stupid("'");
  toml98::enter_inline_array(stupid);

  cr_expect_throw(stupid.stupid_handle_inline_array(), std::runtime_error);
}
