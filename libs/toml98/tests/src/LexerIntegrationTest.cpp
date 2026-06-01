#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "StupidLexer.hpp"

namespace {

std::string read_file(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + path);
  }
  return std::string(std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>());
}

std::vector<toml98::Token*> tokenize_all(const std::string& input) {
  toml98::StupidLexer lex(input);
  std::vector<toml98::Token*> tokens;

  while (lex.stupid_remaining() > 0) {
    try {
      if (lex.stupid_stack().empty()) {
        lex.stupid_stack().push(toml98::LexerNormal);
      }

      toml98::LexerState state = lex.stupid_stack().top();
      toml98::Token* tok = NULL;

      switch (state) {
        case toml98::LexerNormal:
          tok = lex.stupid_handle_normal();
          break;
        case toml98::LexerWord:
          tok = lex.stupid_handle_word();
          break;
        case toml98::LexerString:
          tok = lex.stupid_handle_string();
          break;
        case toml98::LexerStringDouble:
          tok = lex.stupid_handle_string_double();
          break;
        case toml98::LexerStringMultiLine:
          tok = lex.stupid_handle_string_multiline();
          break;
        case toml98::LexerStringDoubleMultiLine:
          tok = lex.stupid_handle_string_double_multiline();
          break;
        case toml98::LexerTableKey:
          tok = lex.stupid_handle_table_key();
          break;
        case toml98::LexerArrayKey:
          tok = lex.stupid_handle_array_key();
          break;
        case toml98::LexerComments:
          tok = lex.stupid_handle_comments();
          break;
        case toml98::LexerInlineArray:
          tok = lex.stupid_handle_inline_array();
          break;
        case toml98::LexerInlineTable:
          tok = lex.stupid_handle_inline_table();
          break;
        case toml98::LexerWhiteSpace:
          tok = lex.stupid_handle_whitespace();
          break;
      }

      if (tok == NULL) {
        continue;
      }

      if (!lex.stupid_stack().empty() &&
          lex.stupid_stack().top() == toml98::LexerComments) {
        lex.stupid_stack().pop();
      }

      if (tok->type == toml98::TokenEqual) {
        lex.stupid_is_last_equal() = true;
      } else if (tok->type != toml98::TokenDelimiter) {
        lex.stupid_is_last_equal() = false;
      }

      tokens.push_back(tok);
    } catch (const std::runtime_error&) {
      break;
    }
  }
  return tokens;
}

struct ExpectedToken {
  toml98::TokenType type;
  std::string value;
};

void assert_token_stream(const std::vector<toml98::Token*>& actual,
                          const std::vector<ExpectedToken>& expected) {
  cr_assert_eq(actual.size(), expected.size(),
               "Token count mismatch: got %zu, expected %zu",
               actual.size(), expected.size());
  for (size_t i = 0; i < actual.size(); i++) {
    cr_expect_eq(actual[i]->type, expected[i].type,
                 "Token %zu type mismatch", i);
    cr_expect_eq(actual[i]->value, expected[i].value,
                 "Token %zu value mismatch", i);
  }
}

void delete_tokens(std::vector<toml98::Token*>& tokens) {
  for (size_t i = 0; i < tokens.size(); i++) {
    delete tokens[i];
  }
}

}  // namespace

Test(lexer_integration, basic_toml) {
  std::string content = read_file("assets/basic.toml");
  std::vector<toml98::Token*> tokens = tokenize_all(content);

  std::vector<ExpectedToken> expected = {
    {toml98::TokenWord, "key"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenString, "value"},
    {toml98::TokenNewLine, "\n"},
  };

  assert_token_stream(tokens, expected);
  delete_tokens(tokens);
}

Test(lexer_integration, table_toml) {
  std::string content = read_file("assets/table.toml");
  std::vector<toml98::Token*> tokens = tokenize_all(content);

  std::vector<ExpectedToken> expected = {
    {toml98::TokenTableStart, "["},
    {toml98::TokenWord, "section"},
    {toml98::TokenTableEnd, "]"},
    {toml98::TokenNewLine, "\n"},
    {toml98::TokenWord, "key"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "123"},
    {toml98::TokenNewLine, "\n"},
  };

  assert_token_stream(tokens, expected);
  delete_tokens(tokens);
}

Test(lexer_integration, comment_toml) {
  std::string content = read_file("assets/comment.toml");
  std::vector<toml98::Token*> tokens = tokenize_all(content);

  std::vector<ExpectedToken> expected = {
    {toml98::TokenNewLine, "\n"},
    {toml98::TokenNewLine, "\n"},
    {toml98::TokenWord, "key"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenString, "value"},
    {toml98::TokenNewLine, "\n"},
  };

  assert_token_stream(tokens, expected);
  delete_tokens(tokens);
}

Test(lexer_integration, nested_tables_toml) {
  std::string content = read_file("assets/nested_tables.toml");
  std::vector<toml98::Token*> tokens = tokenize_all(content);

  std::vector<ExpectedToken> expected = {
    {toml98::TokenTableStart, "["},
    {toml98::TokenWord, "a"},
    {toml98::TokenTableEnd, "]"},
    {toml98::TokenNewLine, "\n"},
    {toml98::TokenTableStart, "["},
    {toml98::TokenWord, "b"},
    {toml98::TokenTableEnd, "]"},
    {toml98::TokenNewLine, "\n"},
    {toml98::TokenWord, "key"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "true"},
    {toml98::TokenNewLine, "\n"},
  };

  assert_token_stream(tokens, expected);
  delete_tokens(tokens);
}

Test(lexer_integration, inline_table_toml) {
  std::string content = read_file("assets/inline_table.toml");
  std::vector<toml98::Token*> tokens = tokenize_all(content);

  std::vector<ExpectedToken> expected = {
    {toml98::TokenWord, "point"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenTableStart, "{"},
    {toml98::TokenWord, "x"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "1"},
    {toml98::TokenTableEnd, "}"},
    {toml98::TokenNewLine, "\n"},
  };

  assert_token_stream(tokens, expected);
  delete_tokens(tokens);
}

Test(lexer_integration, official_example_without_date_toml) {
  std::string content = read_file("assets/official_example_no_date.toml");
  std::vector<toml98::Token*> tokens = tokenize_all(content);

  std::vector<ExpectedToken> expected = {
    {toml98::TokenNewLine, "\n"},
    {toml98::TokenNewLine, "\n"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "title"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenString, "TOML Example"},
    {toml98::TokenNewLine, "\n"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenTableStart, "["},
    {toml98::TokenWord, "owner"},
    {toml98::TokenTableEnd, "]"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "name"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenString, "Tom Preston-Werner"},
    {toml98::TokenNewLine, "\n"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenTableStart, "["},
    {toml98::TokenWord, "database"},
    {toml98::TokenTableEnd, "]"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "enabled"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "true"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "ports"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenArrayStart, "["},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "8000"},
    {toml98::TokenComma, ","},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "8001"},
    {toml98::TokenComma, ","},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "8002"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenArrayEnd, "]"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "data"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenArrayStart, "["},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenArrayStart, "["},
    {toml98::TokenString, "delta"},
    {toml98::TokenComma, ","},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenString, "phi"},
    {toml98::TokenArrayEnd, "]"},
    {toml98::TokenComma, ","},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenArrayStart, "["},
    {toml98::TokenWord, "3"},
    {toml98::TokenDot, "."},
    {toml98::TokenWord, "14"},
    {toml98::TokenArrayEnd, "]"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenArrayEnd, "]"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "temp_targets"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenTableStart, "{"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "cpu"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "79"},
    {toml98::TokenDot, "."},
    {toml98::TokenWord, "5"},
    {toml98::TokenComma, ","},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "case"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "72"},
    {toml98::TokenDot, "."},
    {toml98::TokenWord, "0"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenTableEnd, "}"},
    {toml98::TokenNewLine, "\n"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenTableStart, "["},
    {toml98::TokenWord, "servers"},
    {toml98::TokenTableEnd, "]"},
    {toml98::TokenNewLine, "\n"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenTableStart, "["},
    {toml98::TokenWord, "servers"},
    {toml98::TokenDot, "."},
    {toml98::TokenWord, "alpha"},
    {toml98::TokenTableEnd, "]"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "ip"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenString, "10.0.0.1"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "role"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenString, "frontend"},
    {toml98::TokenNewLine, "\n"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenTableStart, "["},
    {toml98::TokenWord, "servers"},
    {toml98::TokenDot, "."},
    {toml98::TokenWord, "beta"},
    {toml98::TokenTableEnd, "]"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "ip"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenString, "10.0.0.2"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "role"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenString, "backend"},
    {toml98::TokenNewLine, "\n"},
  };

  assert_token_stream(tokens, expected);
  delete_tokens(tokens);
}

Test(lexer_integration, multi_types_toml) {
  std::string content = read_file("assets/multi_types.toml");
  std::vector<toml98::Token*> tokens = tokenize_all(content);

  std::vector<ExpectedToken> expected = {
    {toml98::TokenWord, "string"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenString, "hello"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "integer"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "42"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "float"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "3"},
    {toml98::TokenDot, "."},
    {toml98::TokenWord, "14"},
    {toml98::TokenNewLine, "\n"},

    {toml98::TokenWord, "boolean"},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenEqual, "="},
    {toml98::TokenDelimiter, " "},
    {toml98::TokenWord, "true"},
    {toml98::TokenNewLine, "\n"},
  };

  assert_token_stream(tokens, expected);
  delete_tokens(tokens);
}
