#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <stdint.h>

#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <vector>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "StupidParser.hpp"

using namespace toml98;

Test(readDouble, valid) {
  Token tok = Token(TokenWord, "69");

  std::vector<Token> tokens;
  tokens.push_back(Token(TokenDot, "."));
  tokens.push_back(Token(TokenWord, "621"));

  StupidParser parser = StupidParser(tokens);

  double ret = parser.stupid_readFloat(tok);

  cr_expect(std::abs(ret - 69.621) < std::numeric_limits<double>::epsilon());
}

Test(readDouble, invalid) {
  Token tok = Token(TokenWord, "furaffinity");

  std::vector<Token> tokens;
  tokens.push_back(Token(TokenDot, "."));
  tokens.push_back(Token(TokenWord, "net"));

  StupidParser parser = StupidParser(tokens);

  cr_expect_throw(parser.stupid_readFloat(tok), std::runtime_error);
}

Test(readDouble, invalid_string) {
  // https://www.youtube.com/watch?v=pvy9km7g6fw
  Token tok =
      Token(TokenMultiString,
            "I am the storm that is approaching\nProvoking black clouds in "
            "isolation\nI am reclaimer of my name\nBorn in flames, I have been "
            "blessed\nMy family crest is a demon of death!");

  std::vector<Token> tokens;
  tokens.push_back(tok);

  StupidParser parser;

  cr_expect_throw(parser.stupid_readFloat(tok), std::runtime_error);
}
