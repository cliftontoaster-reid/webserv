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

static void enter_table_key(StupidLexer& stupid) {
  stupid.stupid_stack().push(LexerTableKey);
}

}  // namespace toml98

Test(lexer_handle_table_key, whitespace_pushes_state) {
  toml98::StupidLexer stupid(" ");
  toml98::enter_table_key(stupid);

  toml98::Token* token = stupid.stupid_handle_table_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWhiteSpace);
}

Test(lexer_handle_table_key, tab_pushes_state) {
  toml98::StupidLexer stupid("\t");
  toml98::enter_table_key(stupid);

  toml98::Token* token = stupid.stupid_handle_table_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWhiteSpace);
}

Test(lexer_handle_table_key, dot_token) {
  toml98::StupidLexer stupid(".");
  toml98::enter_table_key(stupid);

  toml98::Token* token = stupid.stupid_handle_table_key();
  toml98::expect_token(token, toml98::TokenDot, ".");
  cr_expect_eq(stupid.stupid_pos(), 1U);
}

Test(lexer_handle_table_key, right_bracket_ends_key) {
  toml98::StupidLexer stupid("]");
  toml98::enter_table_key(stupid);

  toml98::Token* token = stupid.stupid_handle_table_key();
  toml98::expect_token(token, toml98::TokenTableEnd, "]");
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
}

Test(lexer_handle_table_key, word_state_from_alnum) {
  toml98::StupidLexer stupid("a");
  toml98::enter_table_key(stupid);

  toml98::Token* token = stupid.stupid_handle_table_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}

Test(lexer_handle_table_key, word_state_from_hyphen) {
  toml98::StupidLexer stupid("-");
  toml98::enter_table_key(stupid);

  toml98::Token* token = stupid.stupid_handle_table_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}

Test(lexer_handle_table_key, word_state_from_underscore) {
  toml98::StupidLexer stupid("_");
  toml98::enter_table_key(stupid);

  toml98::Token* token = stupid.stupid_handle_table_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 0U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerWord);
}

Test(lexer_handle_table_key, double_quote_string_state) {
  toml98::StupidLexer stupid("\"a");
  toml98::enter_table_key(stupid);

  toml98::Token* token = stupid.stupid_handle_table_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerStringDouble);
}

Test(lexer_handle_table_key, single_quote_string_state) {
  toml98::StupidLexer stupid("'a");
  toml98::enter_table_key(stupid);

  toml98::Token* token = stupid.stupid_handle_table_key();
  cr_expect_null(token);
  cr_expect_eq(stupid.stupid_pos(), 1U);

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerString);
}

Test(lexer_handle_table_key, multiline_double_quote_throws) {
  toml98::StupidLexer stupid("\"\"\"");
  toml98::enter_table_key(stupid);

  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, multiline_single_quote_throws) {
  toml98::StupidLexer stupid("'''");
  toml98::enter_table_key(stupid);

  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, unterminated_quote_throws) {
  toml98::StupidLexer stupid("\"");
  toml98::enter_table_key(stupid);

  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, unknown_character_throws) {
  toml98::StupidLexer stupid(",");
  toml98::enter_table_key(stupid);

  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}

Test(lexer_handle_table_key, eof_throws) {
  toml98::StupidLexer stupid("");
  toml98::enter_table_key(stupid);

  cr_expect_throw(stupid.stupid_handle_table_key(), std::runtime_error);
}
