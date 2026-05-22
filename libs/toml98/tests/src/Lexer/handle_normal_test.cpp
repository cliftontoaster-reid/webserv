#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>

#include "Lexer.hpp"

namespace toml98 {

class StupidNormalLexer : public Lexer {
 public:
  StupidNormalLexer() {};
  explicit StupidNormalLexer(const std::string& str) : Lexer(str) {}

  std::stack<LexerState>& stupidState() { return _stack; }
  Token* stupid_normal() { return handle_normal(); }
};

}  // namespace toml98

Test(lexer_handle_normal, whitespace) {
  toml98::StupidNormalLexer stupid = toml98::StupidNormalLexer(" ");
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
  toml98::StupidNormalLexer stupid = toml98::StupidNormalLexer("a");
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
  toml98::StupidNormalLexer stupid = toml98::StupidNormalLexer("7");
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
  toml98::StupidNormalLexer stupid = toml98::StupidNormalLexer("#");
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
  toml98::StupidNormalLexer stupid = toml98::StupidNormalLexer("\t");
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
  toml98::StupidNormalLexer stupid = toml98::StupidNormalLexer("{");
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
  toml98::StupidNormalLexer stupid = toml98::StupidNormalLexer("\"x");
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
  toml98::StupidNormalLexer stupid = toml98::StupidNormalLexer("\"\"\"");
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
  toml98::StupidNormalLexer stupid = toml98::StupidNormalLexer("'x");
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
  toml98::StupidNormalLexer stupid = toml98::StupidNormalLexer("'''");
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

Test(lexer_handle_normal, unknown_character) {
  toml98::StupidNormalLexer stupid = toml98::StupidNormalLexer(",");
  cr_expect_throw(stupid.stupid_normal(), std::runtime_error);
}
