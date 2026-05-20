#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>

#include "Lexer.hpp"

class StupidNormalLexer : public toml98::Lexer {
 public:
  StupidNormalLexer() {};
  explicit StupidNormalLexer(const std::string& str) : toml98::Lexer(str) {}

  std::stack<toml98::LexerState>& stupidState() { return _stack; }
  toml98::Token* stupid_normal() { return handle_normal(); }
};

Test(lexer_handle_normal, whitespace) {
  StupidNormalLexer stupid = StupidNormalLexer(" ");
  stupid.stupid_normal();

  toml98::LexerState expected = toml98::LexerWhiteSpace;

  auto state = stupid.stupidState();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, alpha) {
  StupidNormalLexer stupid = StupidNormalLexer("a");
  stupid.stupid_normal();

  toml98::LexerState expected = toml98::LexerWord;

  auto state = stupid.stupidState();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, numeric) {
  StupidNormalLexer stupid = StupidNormalLexer("7");
  stupid.stupid_normal();

  toml98::LexerState expected = toml98::LexerWord;

  auto state = stupid.stupidState();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, comment) {
  StupidNormalLexer stupid = StupidNormalLexer("#");
  stupid.stupid_normal();

  toml98::LexerState expected = toml98::LexerComments;

  auto state = stupid.stupidState();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, tab_whitespace) {
  StupidNormalLexer stupid = StupidNormalLexer("\t");
  stupid.stupid_normal();

  toml98::LexerState expected = toml98::LexerWhiteSpace;

  auto state = stupid.stupidState();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, inline_table) {
  StupidNormalLexer stupid = StupidNormalLexer("{");
  stupid.stupid_normal();

  toml98::LexerState expected = toml98::LexerInlineTable;

  auto state = stupid.stupidState();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, string_double) {
  StupidNormalLexer stupid = StupidNormalLexer("\"x");
  stupid.stupid_normal();

  toml98::LexerState expected = toml98::LexerStringDouble;

  auto state = stupid.stupidState();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, string_double_multiline) {
  StupidNormalLexer stupid = StupidNormalLexer("\"\"\"");
  stupid.stupid_normal();

  toml98::LexerState expected = toml98::LexerStringDoubleMultiLine;

  auto state = stupid.stupidState();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, string_single) {
  StupidNormalLexer stupid = StupidNormalLexer("'x");
  stupid.stupid_normal();

  toml98::LexerState expected = toml98::LexerString;

  auto state = stupid.stupidState();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, string_single_multiline) {
  StupidNormalLexer stupid = StupidNormalLexer("'''");
  stupid.stupid_normal();

  toml98::LexerState expected = toml98::LexerStringMultiLine;

  auto state = stupid.stupidState();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), expected);
  state.pop();
  cr_expect_not(state.empty());
  cr_expect_eq(state.top(), toml98::LexerNormal);
  state.pop();
  cr_expect(state.empty());
}

Test(lexer_handle_normal, double_quote_error) {
  StupidNormalLexer stupid = StupidNormalLexer("\"\"x");
  cr_expect_throw(stupid.stupid_normal(), std::runtime_error);
}

Test(lexer_handle_normal, single_quote_error) {
  StupidNormalLexer stupid = StupidNormalLexer("''x");
  cr_expect_throw(stupid.stupid_normal(), std::runtime_error);
}

Test(lexer_handle_normal, unknown_character) {
  StupidNormalLexer stupid = StupidNormalLexer(",");
  cr_expect_throw(stupid.stupid_normal(), std::runtime_error);
}

Test(lexer_handle_normal, newline) {
  StupidNormalLexer stupid = StupidNormalLexer("\n");
  cr_expect_throw(stupid.stupid_normal(), std::runtime_error);
}
