#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <iostream>
#include <stack>

#include "Lexer.hpp"

class StupidNormalLexer : public toml98::Lexer {
 public:
  StupidNormalLexer() {};
  explicit StupidNormalLexer(const std::string& str) : toml98::Lexer(str) {}

  std::stack<toml98::LexerState>& stupidState() { return _stack; }
  toml98::Token* stupid_normal() { return handle_normal(); }
};

Test(lexer_handle_normal, whitespace) {
  StupidNormalLexer stupid = StupidNormalLexer("  ");
  stupid.stupid_normal();

  auto state = stupid.stupidState();
  cr_expect_eq(state.top(), toml98::LexerWhiteSpace);
  state.pop();
  cr_expect_eq(state.top(), toml98::LexerNormal);
}
