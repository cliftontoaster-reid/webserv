#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>

#include "Lexer.hpp"
#include "StupidLexer.hpp"

Test(lexer_handle_string_double, empty) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDouble);

  res = stupid.stupid_handle_string_double();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "");

  delete res;
}

Test(lexer_handle_string_double, whitespace) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\" \t \t \"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDouble);

  res = stupid.stupid_handle_string_double();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, " \t \t ");

  delete res;
}

Test(lexer_handle_string_double, eeof) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"PLEASE HELP ME I-");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDouble);

  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, newline_cr) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"\r\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDouble);

  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, newline_ln) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"\n\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDouble);

  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, newline_crln) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"\r\n\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDouble);

  cr_expect_throw(stupid.stupid_handle_string_double(), std::runtime_error);
}

Test(lexer_handle_string_double, beginner) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"Hello World!\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDouble);

  res = stupid.stupid_handle_string_double();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "Hello World!");

  delete res;
}

Test(lexer_handle_string_double, unicode) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"I  Unicode\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDouble);

  res = stupid.stupid_handle_string_double();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "I  Unicode");

  delete res;
}

Test(lexer_handle_string_double, true_newline_ln) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid =
      toml98::StupidLexer("\"PLEASE HELP ME I-\\n wait…I am alive!\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDouble);

  res = stupid.stupid_handle_string_double();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "PLEASE HELP ME I-\n wait…I am alive!");

  delete res;
}

Test(lexer_handle_string_double, spam_escape) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid =
      toml98::StupidLexer("\"\\n\\n\\n\\n\\n\\n\\n\\n\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDouble);

  res = stupid.stupid_handle_string_double();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "\n\n\n\n\n\n\n\n");

  delete res;
}

Test(lexer_handle_string_double, escape_single) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid =
      toml98::StupidLexer("\"\\b\\t\\n\\f\\r\\e\\\"\\\\\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDouble);

  res = stupid.stupid_handle_string_double();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "\b\t\n\f\r\e\"\\");

  delete res;
}

Test(lexer_handle_string_double, escape_chess) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer(
      "\"These are some chess pieces for NerdFont : \\ue29c \\ue25f \\ue260\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDouble);

  res = stupid.stupid_handle_string_double();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value,
               "These are some chess pieces for NerdFont :   ");

  delete res;
}
