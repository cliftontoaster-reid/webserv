#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "StupidParser.hpp"
#include "Value.hpp"

using namespace toml98;

Test(readBody, valid_cringe) {
  std::vector<Token> tokens;

  tokens.push_back(Token(TokenWord, "furries"));
  tokens.push_back(Token(TokenEqual, "="));
  tokens.push_back(Token(TokenString, "cringe"));

  StupidParser par;

  Value res = par.stupid_parse(tokens);

  check_entry(__FILE__, __LINE__, res, "cringe", "furries");
}

static inline void push_entry(const std::string& key, const std::string& value,
                              std::vector<Token>& tokens) {
  tokens.push_back(Token(TokenWord, key));
  tokens.push_back(Token(TokenEqual, "="));
  tokens.push_back(Token(TokenString, value));
}

Test(readBody, valid_translate) {
  std::vector<Token> tokens;
  push_entry("hello", "こんにちは", tokens);
  push_entry("yes", "はい", tokens);
  push_entry("no", "いいえ", tokens);
  push_entry("goodbye", "さようなら", tokens);

  StupidParser par;

  Value res = par.stupid_parse(tokens);

  check_entry(__FILE__, __LINE__, res, "こんにちは", "hello");
  check_entry(__FILE__, __LINE__, res, "はい", "yes");
  check_entry(__FILE__, __LINE__, res, "いいえ", "no");
  check_entry(__FILE__, __LINE__, res, "さようなら", "goodbye");
}
