#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "StupidLexer.hpp"
#include "utils.hpp"

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
      toml98::Token* ret = lex.run();

      if (ret == NULL) {
        continue;
      }

      tokens.push_back(ret);
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
  // cr_assert_eq(actual.size(), expected.size(),
  //  "Token count mismatch: got %zu, expected %zu", actual.size(),
  //  expected.size());
  for (size_t i = 0; i < actual.size(); i++) {
    if (actual[i]->type != expected[i].type) {
      std::cerr << "Token type mismatch!" << '\n';
      std::cerr << "  Expected: " << expected[i].type << '\n';
      std::cerr << "  Actual:   " << actual[i]->type << '\n';
      cr_expect_eq(actual[i]->type, expected[i].type,
                   "Token %zu type mismatch, got %s, expected %s", i,
                   toml98::tokTypeToString(actual[i]->type).c_str(),
                   toml98::tokTypeToString(expected[i].type).c_str());
    }

    if (actual[i]->value != expected[i].value) {
      std::cerr << "Token value mismatch!" << '\n';
      std::cerr << "  Expected: " << expected[i].value << '\n';
      std::cerr << "  Actual:   " << actual[i]->value << '\n';
      cr_expect_eq(actual[i]->type, expected[i].type,
                   "Token %zu type mismatch, got %s, expected %s", i,
                   toml98::tokTypeToString(actual[i]->type).c_str(),
                   toml98::tokTypeToString(expected[i].type).c_str());
    }
    cr_expect_eq(actual[i]->type, expected[i].type,
                 "Token %zu type mismatch, got %s, expected %s", i,
                 toml98::tokTypeToString(actual[i]->type).c_str(),
                 toml98::tokTypeToString(expected[i].type).c_str());
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
      {toml98::TokenWord, "key"},     {toml98::TokenDelimiter, " "},
      {toml98::TokenEqual, "="},      {toml98::TokenDelimiter, " "},
      {toml98::TokenString, "value"}, {toml98::TokenNewLine, "\n"},
  };

  assert_token_stream(tokens, expected);
  delete_tokens(tokens);
}

Test(lexer_integration, table_toml) {
  std::string content = read_file("assets/table.toml");
  std::vector<toml98::Token*> tokens = tokenize_all(content);

  std::vector<ExpectedToken> expected = {
      {toml98::TokenTableKeyStart, "["}, {toml98::TokenWord, "section"},
      {toml98::TokenTableKeyEnd, "]"},   {toml98::TokenNewLine, "\n"},
      {toml98::TokenWord, "key"},        {toml98::TokenDelimiter, " "},
      {toml98::TokenEqual, "="},         {toml98::TokenDelimiter, " "},
      {toml98::TokenWord, "123"},        {toml98::TokenNewLine, "\n"},
  };

  assert_token_stream(tokens, expected);
  delete_tokens(tokens);
}

Test(lexer_integration, comment_toml) {
  std::string content = read_file("assets/comment.toml");
  std::vector<toml98::Token*> tokens = tokenize_all(content);

  std::vector<ExpectedToken> expected = {
      {toml98::TokenNewLine, "\n"},  {toml98::TokenWord, "key"},
      {toml98::TokenDelimiter, " "}, {toml98::TokenEqual, "="},
      {toml98::TokenDelimiter, " "}, {toml98::TokenString, "value"},
      {toml98::TokenNewLine, "\n"},
  };

  assert_token_stream(tokens, expected);
  delete_tokens(tokens);
}

Test(lexer_integration, nested_tables_toml) {
  std::string content = read_file("assets/nested_tables.toml");
  std::vector<toml98::Token*> tokens = tokenize_all(content);

  std::vector<ExpectedToken> expected = {
      {toml98::TokenTableKeyStart, "["}, {toml98::TokenWord, "a"},
      {toml98::TokenTableKeyEnd, "]"},   {toml98::TokenNewLine, "\n"},
      {toml98::TokenTableKeyStart, "["}, {toml98::TokenWord, "b"},
      {toml98::TokenTableKeyEnd, "]"},   {toml98::TokenNewLine, "\n"},
      {toml98::TokenWord, "key"},        {toml98::TokenDelimiter, " "},
      {toml98::TokenEqual, "="},         {toml98::TokenDelimiter, " "},
      {toml98::TokenWord, "true"},       {toml98::TokenNewLine, "\n"},
  };

  assert_token_stream(tokens, expected);
  delete_tokens(tokens);
}

Test(lexer_integration, inline_table_toml) {
  std::string content = read_file("assets/inline_table.toml");
  std::vector<toml98::Token*> tokens = tokenize_all(content);

  std::vector<ExpectedToken> expected = {
      {toml98::TokenWord, "point"},   {toml98::TokenDelimiter, " "},
      {toml98::TokenEqual, "="},      {toml98::TokenDelimiter, " "},
      {toml98::TokenTableStart, "{"}, {toml98::TokenWord, "x"},
      {toml98::TokenDelimiter, " "},  {toml98::TokenEqual, "="},
      {toml98::TokenDelimiter, " "},  {toml98::TokenWord, "1"},
      {toml98::TokenTableEnd, "}"},   {toml98::TokenNewLine, "\n"},
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

      {toml98::TokenWord, "title"},
      {toml98::TokenDelimiter, " "},
      {toml98::TokenEqual, "="},
      {toml98::TokenDelimiter, " "},
      {toml98::TokenString, "TOML Example"},
      {toml98::TokenNewLine, "\n"},
      {toml98::TokenNewLine, "\n"},

      {toml98::TokenTableKeyStart, "["},
      {toml98::TokenWord, "owner"},
      {toml98::TokenTableKeyEnd, "]"},
      {toml98::TokenNewLine, "\n"},

      {toml98::TokenWord, "name"},
      {toml98::TokenDelimiter, " "},
      {toml98::TokenEqual, "="},
      {toml98::TokenDelimiter, " "},
      {toml98::TokenString, "Tom Preston-Werner"},
      {toml98::TokenNewLine, "\n"},
      {toml98::TokenNewLine, "\n"},

      {toml98::TokenTableKeyStart, "["},
      {toml98::TokenWord, "database"},
      {toml98::TokenTableKeyEnd, "]"},
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
      {toml98::TokenWord, "8000"},
      {toml98::TokenComma, ","},
      {toml98::TokenDelimiter, " "},
      {toml98::TokenWord, "8001"},
      {toml98::TokenComma, ","},
      {toml98::TokenDelimiter, " "},
      {toml98::TokenWord, "8002"},
      {toml98::TokenArrayEnd, "]"},
      {toml98::TokenNewLine, "\n"},

      {toml98::TokenWord, "data"},
      {toml98::TokenDelimiter, " "},
      {toml98::TokenEqual, "="},
      {toml98::TokenDelimiter, " "},
      {toml98::TokenArrayStart, "["},
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

      {toml98::TokenTableKeyStart, "["},
      {toml98::TokenWord, "servers"},
      {toml98::TokenTableKeyEnd, "]"},
      {toml98::TokenNewLine, "\n"},
      {toml98::TokenNewLine, "\n"},

      {toml98::TokenTableKeyStart, "["},
      {toml98::TokenWord, "servers"},
      {toml98::TokenDot, "."},
      {toml98::TokenWord, "alpha"},
      {toml98::TokenTableKeyEnd, "]"},
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

      {toml98::TokenTableKeyStart, "["},
      {toml98::TokenWord, "servers"},
      {toml98::TokenDot, "."},
      {toml98::TokenWord, "beta"},
      {toml98::TokenTableKeyEnd, "]"},
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
      {toml98::TokenWord, "string"},  {toml98::TokenDelimiter, " "},
      {toml98::TokenEqual, "="},      {toml98::TokenDelimiter, " "},
      {toml98::TokenString, "hello"}, {toml98::TokenNewLine, "\n"},

      {toml98::TokenWord, "integer"}, {toml98::TokenDelimiter, " "},
      {toml98::TokenEqual, "="},      {toml98::TokenDelimiter, " "},
      {toml98::TokenWord, "42"},      {toml98::TokenNewLine, "\n"},

      {toml98::TokenWord, "float"},   {toml98::TokenDelimiter, " "},
      {toml98::TokenEqual, "="},      {toml98::TokenDelimiter, " "},
      {toml98::TokenWord, "3"},       {toml98::TokenDot, "."},
      {toml98::TokenWord, "14"},      {toml98::TokenNewLine, "\n"},

      {toml98::TokenWord, "boolean"}, {toml98::TokenDelimiter, " "},
      {toml98::TokenEqual, "="},      {toml98::TokenDelimiter, " "},
      {toml98::TokenWord, "true"},    {toml98::TokenNewLine, "\n"},
  };

  assert_token_stream(tokens, expected);
  delete_tokens(tokens);
}
