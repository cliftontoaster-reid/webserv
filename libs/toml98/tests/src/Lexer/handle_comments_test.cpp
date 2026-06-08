#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>

#include "StupidLexer.hpp"

namespace toml98 {

static void expect_newline_token(Token* token) { cr_assert_null(token); }

}  // namespace toml98

Test(lexer_handle_comments, stops_before_newline) {
  toml98::StupidLexer stupid("#abc\n");

  toml98::Token* token = stupid.stupid_handle_comments();
  toml98::expect_newline_token(token);

  cr_expect_eq(stupid.stupid_pos(), 4U);
  cr_expect_eq(stupid.stupid_peek(), '\n');
}

Test(lexer_handle_comments, consumes_until_eof) {
  toml98::StupidLexer stupid("#abc");

  toml98::Token* token = stupid.stupid_handle_comments();
  toml98::expect_newline_token(token);

  cr_expect_eq(stupid.stupid_pos(), 4U);
  cr_expect_eq(stupid.stupid_remaining(), 0U);
}

Test(lexer_handle_comments, empty_comment) {
  toml98::StupidLexer stupid("#");

  toml98::Token* token = stupid.stupid_handle_comments();
  toml98::expect_newline_token(token);

  cr_expect_eq(stupid.stupid_pos(), 1U);
  cr_expect_eq(stupid.stupid_remaining(), 0U);
}
