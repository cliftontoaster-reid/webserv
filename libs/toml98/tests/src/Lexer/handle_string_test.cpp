#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>

#include "Lexer.hpp"
#include "StupidLexer.hpp"

Test(lexer_handle_string, empty) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("''");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerString);

  res = stupid.stupid_handle_string();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "");

  delete res;
}

Test(lexer_handle_string, whitespace) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("' \t \t '");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerString);

  res = stupid.stupid_handle_string();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, " \t \t ");

  delete res;
}

Test(lexer_handle_string, eeof) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("'PLEASE HELP ME I-");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerString);

  cr_expect_throw(stupid.stupid_handle_string(), std::runtime_error);
}

Test(lexer_handle_string, newline_cr) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("'\r'");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerString);

  cr_expect_throw(stupid.stupid_handle_string(), std::runtime_error);
}

Test(lexer_handle_string, newline_ln) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("'\n'");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerString);

  cr_expect_throw(stupid.stupid_handle_string(), std::runtime_error);
}

Test(lexer_handle_string, newline_crln) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("'\r\n'");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerString);

  cr_expect_throw(stupid.stupid_handle_string(), std::runtime_error);
}

Test(lexer_handle_string, beginner) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("'Hello World!'");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerString);

  res = stupid.stupid_handle_string();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "Hello World!");

  delete res;
}

Test(lexer_handle_string, unicode) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("'I  Unicode'");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerString);

  res = stupid.stupid_handle_string();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "I  Unicode");

  delete res;
}

Test(lexer_handle_string, false_newline_ln) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid =
      toml98::StupidLexer("'PLEASE HELP ME I-\\n wait…I am alive!'");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerString);

  res = stupid.stupid_handle_string();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "PLEASE HELP ME I-\\n wait…I am alive!");

  delete res;
}

Test(lexer_handle_string, spam_escape) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid =
      toml98::StupidLexer("'\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\'");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerString);

  res = stupid.stupid_handle_string();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");

  delete res;
}
