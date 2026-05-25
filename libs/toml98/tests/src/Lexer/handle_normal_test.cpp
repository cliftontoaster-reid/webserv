#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>

#include "StupidLexer.hpp"

Test(lexer_handle_normal, whitespace) {
  toml98::StupidLexer stupid = toml98::StupidLexer(" ");
  stupid.stupid_handle_normal();

  toml98::LexerState expected = toml98::LexerWhiteSpace;

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, alpha) {
  toml98::StupidLexer stupid = toml98::StupidLexer("a");
  stupid.stupid_handle_normal();

  toml98::LexerState expected = toml98::LexerWord;

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, numeric) {
  toml98::StupidLexer stupid = toml98::StupidLexer("7");
  stupid.stupid_handle_normal();

  toml98::LexerState expected = toml98::LexerWord;

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, comment) {
  toml98::StupidLexer stupid = toml98::StupidLexer("#");
  stupid.stupid_handle_normal();

  toml98::LexerState expected = toml98::LexerComments;

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, tab_whitespace) {
  toml98::StupidLexer stupid = toml98::StupidLexer("\t");
  stupid.stupid_handle_normal();

  toml98::LexerState expected = toml98::LexerWhiteSpace;

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, inline_table) {
  toml98::StupidLexer stupid = toml98::StupidLexer("{");
  stupid.stupid_handle_normal();

  toml98::LexerState expected = toml98::LexerInlineTable;

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, string_double) {
  toml98::StupidLexer stupid = toml98::StupidLexer("\"x");
  stupid.stupid_handle_normal();

  toml98::LexerState expected = toml98::LexerStringDouble;

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, string_double_multiline) {
  toml98::StupidLexer stupid = toml98::StupidLexer("\"\"\"");
  stupid.stupid_handle_normal();

  toml98::LexerState expected = toml98::LexerStringDoubleMultiLine;

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, string_single) {
  toml98::StupidLexer stupid = toml98::StupidLexer("'x");
  stupid.stupid_handle_normal();

  toml98::LexerState expected = toml98::LexerString;

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, string_single_multiline) {
  toml98::StupidLexer stupid = toml98::StupidLexer("'''");
  stupid.stupid_handle_normal();

  toml98::LexerState expected = toml98::LexerStringMultiLine;

  auto state = stupid.stupid_stack();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, unknown_character) {
  toml98::StupidLexer stupid = toml98::StupidLexer(",");
  cr_expect_throw(stupid.stupid_handle_normal(), std::runtime_error);
}
