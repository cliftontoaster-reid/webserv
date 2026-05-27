#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stack>
#include <stdexcept>

#include "Lexer.hpp"
#include "StupidLexer.hpp"

Test(lexer_handle_string_double_multiline, empty) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"\"\"\"\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  res = stupid.stupid_handle_string_double_multiline();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "");

  delete res;
}

Test(lexer_handle_string_double_multiline, early_newline) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"\"\"\n\"\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  res = stupid.stupid_handle_string_double_multiline();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "");

  delete res;
}

Test(lexer_handle_string_double_multiline, whitespace) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"\"\"\n \t \t \"\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  res = stupid.stupid_handle_string_double_multiline();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, " \t \t ");

  delete res;
}

Test(lexer_handle_string_double_multiline, newline_whitespace) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid =
      toml98::StupidLexer("\"\"\"\n \t\n \t\r\n \"\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  res = stupid.stupid_handle_string_double_multiline();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, " \t\n \t\r\n ");

  delete res;
}

Test(lexer_handle_string_double_multiline, eeof) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid =
      toml98::StupidLexer("\"\"\"\nWAIT NO NOT AGAIN NOOOOOO");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  cr_expect_throw(stupid.stupid_handle_string_double_multiline(),
                  std::runtime_error);
}

Test(lexer_handle_string_double_multiline, newline_cr) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"\"\"\r\r\r\"\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  res = stupid.stupid_handle_string_double_multiline();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "\r\r");

  delete res;
}

Test(lexer_handle_string_double_multiline, newline_ln) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"\"\"\n\n\n\"\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  res = stupid.stupid_handle_string_double_multiline();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "\n\n");

  delete res;
}

Test(lexer_handle_string_double_multiline, newline_crln) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer("\"\"\"\r\n\r\n\r\n\"\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  res = stupid.stupid_handle_string_double_multiline();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "\r\n\r\n");

  delete res;
}

Test(lexer_handle_string_double_multiline, beginner) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid =
      toml98::StupidLexer("\"\"\"\r\nHello World!\r\nHello World!\r\n\"\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  res = stupid.stupid_handle_string_double_multiline();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "Hello World!\r\nHello World!\r\n");

  delete res;
}

Test(lexer_handle_string_double_multiline, unicode) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid =
      toml98::StupidLexer("\"\"\"\r\nI  Unicode\r\nI  Unicode\r\n\"\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  res = stupid.stupid_handle_string_double_multiline();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "I  Unicode\r\nI  Unicode\r\n");

  delete res;
}

Test(lexer_handle_string_double_multiline, spam_escape) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer(
      "\"\"\"\\n\n\\n\n\\n\n\\n\n\\n\n\\n\n\\n\n\\n\n\"\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  res = stupid.stupid_handle_string_double_multiline();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

  delete res;
}

Test(lexer_handle_string_double_multiline, escape_single) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid =
      toml98::StupidLexer("\"\"\"\n\\b\n\\t\n\\n\n\\f\n\\r\\e\\\"\\\\\"\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  res = stupid.stupid_handle_string_double_multiline();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(res->value, "\b\n\t\n\n\n\f\n\r\e\"\\");

  delete res;
}

Test(lexer_handle_string_double_multiline, escape_chess) {
  toml98::Token* res = NULL;
  toml98::StupidLexer stupid = toml98::StupidLexer(
      "\"\"\"These are some chess pieces for NerdFont : \n- \\ue29c \n- "
      "\\ue25f \n- \\ue260\"\"\"");

  res = stupid.stupid_handle_normal();

  cr_expect_null(res);

  toml98::LexerState last = stupid.stupid_stack().top();
  cr_expect_eq(last, toml98::LexerStringDoubleMultiLine);

  res = stupid.stupid_handle_string_double_multiline();

  cr_expect_not_null(res);

  cr_expect_eq(res->type, toml98::TokenString);
  cr_expect_eq(
      res->value,
      "These are some chess pieces for NerdFont : \n-  \n-  \n- ");

  delete res;
}