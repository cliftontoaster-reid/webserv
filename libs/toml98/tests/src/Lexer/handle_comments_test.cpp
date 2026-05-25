#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>
#include <string>

#include "StupidLexer.hpp"

using namespace toml98;

static void expect_newline_token(Token* token) {
  cr_assert_not_null(token);
  cr_assert_eq(token->type, TokenNewLine);
  delete token;
}

Test(lexer_handle_comments, simple_text) {
  StupidLexer stupid(" comment\n");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
  cr_assert_eq(stupid.stupid_pos(), 9);
}

Test(lexer_handle_comments, with_spaces) {
  StupidLexer stupid("  comment with spaces\n");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
  cr_assert_eq(stupid.stupid_pos(), 22);
}

Test(lexer_handle_comments, with_special_chars) {
  StupidLexer stupid(" !@#$%^&*()\n");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
  cr_assert_eq(stupid.stupid_pos(), 12);
}

Test(lexer_handle_comments, empty_comment) {
  StupidLexer stupid("\n");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
  cr_assert_eq(stupid.stupid_pos(), 1);
}

Test(lexer_handle_comments, with_unicode) {
  StupidLexer stupid(" héllo wörld\n");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
}

Test(lexer_handle_comments, with_tabs) {
  StupidLexer stupid("\tindented\tcomment\n");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
  cr_assert_eq(stupid.stupid_pos(), 19);
}

Test(lexer_handle_comments, eof_no_newline) {
  StupidLexer stupid(" just text");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
  cr_assert_eq(stupid.stupid_pos(), 10);
}

Test(lexer_handle_comments, multiple_hashes) {
  StupidLexer stupid("## nested ##\n");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
  cr_assert_eq(stupid.stupid_pos(), 13);
}

Test(lexer_handle_comments, comment_with_numbers) {
  StupidLexer stupid(" key = \"value\"\n");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
  cr_assert_eq(stupid.stupid_pos(), 15);
}

Test(lexer_handle_comments, comment_at_eof_no_newline) {
  StupidLexer stupid(" final comment");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
  cr_assert_eq(stupid.stupid_pos(), 14);
}

Test(lexer_handle_comments, very_long_comment) {
  std::string input = " " + std::string(10000, 'a') + "\n";
  StupidLexer stupid(input);
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
  cr_assert_eq(stupid.stupid_pos(), 10002);
}

Test(lexer_handle_comments, null_character_in_comment) {
  std::string input = " hello\0 world\n";
  StupidLexer stupid(input);
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
}

Test(lexer_handle_comments, cr_ends_comment) {
  StupidLexer stupid(" hello\r");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
}

Test(lexer_handle_comments, comment_with_brackets) {
  StupidLexer stupid(" [table] key\n");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
  cr_assert_eq(stupid.stupid_pos(), 13);
}

Test(lexer_handle_comments, comment_with_quotes) {
  StupidLexer stupid(" \"unterminated string\n");
  Token* token = stupid.stupid_handle_comments();
  expect_newline_token(token);
  cr_assert_eq(stupid.stupid_pos(), 22);
}
