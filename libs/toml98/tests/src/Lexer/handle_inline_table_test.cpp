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

static void enter_inline_table(StupidLexer& stupid) {
  stupid.stupid_stack().push(LexerInlineTable);
}

}  // namespace toml98

Test(lexer_handle_inline_table, comment_pushes_state) {
  toml98::StupidLexer stupid("#");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerComments);
}

Test(lexer_handle_inline_table, whitespace_pushes_state) {
  toml98::StupidLexer stupid(" ");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWhiteSpace);
}

Test(lexer_handle_inline_table, tab_pushes_state) {
  toml98::StupidLexer stupid("\t");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWhiteSpace);
}

Test(lexer_handle_inline_table, double_quote_string_state) {
  toml98::StupidLexer stupid("\"a");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerStringDouble);
}

Test(lexer_handle_inline_table, double_quote_multiline_string_state) {
  toml98::StupidLexer stupid("\"\"\"");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 3U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerStringDoubleMultiLine);
}

Test(lexer_handle_inline_table, single_quote_string_state) {
  toml98::StupidLexer stupid("'a");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerString);
}

Test(lexer_handle_inline_table, single_quote_multiline_string_state) {
  toml98::StupidLexer stupid("'''");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 3U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerStringMultiLine);
}

Test(lexer_handle_inline_table, nested_inline_table_token) {
  toml98::StupidLexer stupid("{");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  toml98::expect_token(token, toml98::TokenTableStart, "{");
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerInlineTable);
}

Test(lexer_handle_inline_table, inline_array_token) {
  toml98::StupidLexer stupid("[");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  toml98::expect_token(token, toml98::TokenArrayStart, "[");
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerInlineArray);
}

Test(lexer_handle_inline_table, array_key_token) {
  toml98::StupidLexer stupid("[[");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  toml98::expect_token(token, toml98::TokenArrayStart, "[[");
  cr_expect_eq(stupid.stupid_pos(), 2U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerArrayKey);
}

Test(lexer_handle_inline_table, colon_token) {
  toml98::StupidLexer stupid("=");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  toml98::expect_token(token, toml98::TokenEqual, "=");
  cr_expect_eq(stupid.stupid_pos(), 1U);
}

Test(lexer_handle_inline_table, word_state_from_alnum) {
  toml98::StupidLexer stupid("a");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}

Test(lexer_handle_inline_table, word_state_from_hyphen) {
  toml98::StupidLexer stupid("-");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}

Test(lexer_handle_inline_table, word_state_from_underscore) {
  toml98::StupidLexer stupid("_");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}

Test(lexer_handle_inline_table, dot_token) {
  toml98::StupidLexer stupid(".");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  toml98::expect_token(token, toml98::TokenDot, ".");
  cr_expect_eq(stupid.stupid_pos(), 1U);
}

Test(lexer_handle_inline_table, newline_token) {
  toml98::StupidLexer stupid("\n");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  toml98::expect_token(token, toml98::TokenNewLine, "\n");
  cr_expect_eq(stupid.stupid_pos(), 1U);
}

Test(lexer_handle_inline_table, crlf_token) {
  toml98::StupidLexer stupid("\r\n");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  toml98::expect_token(token, toml98::TokenNewLine, "\r\n");
  cr_expect_eq(stupid.stupid_pos(), 2U);
}

Test(lexer_handle_inline_table, carriage_return_throws) {
  toml98::StupidLexer stupid("\r");
  toml98::enter_inline_table(stupid);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, comma_token) {
  toml98::StupidLexer stupid(",");
  toml98::enter_inline_table(stupid);

  toml98::Token* token = stupid.stupid_handle_inline_table();
  toml98::expect_token(token, toml98::TokenComma, ",");
  cr_expect_eq(stupid.stupid_pos(), 1U);
}

Test(lexer_handle_inline_table, unknown_character_throws) {
  toml98::StupidLexer stupid("?");
  toml98::enter_inline_table(stupid);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, unterminated_double_quote_throws) {
  toml98::StupidLexer stupid("\"");
  toml98::enter_inline_table(stupid);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}

Test(lexer_handle_inline_table, unterminated_single_quote_throws) {
  toml98::StupidLexer stupid("'");
  toml98::enter_inline_table(stupid);

  cr_expect_throw(stupid.stupid_handle_inline_table(), std::runtime_error);
}
