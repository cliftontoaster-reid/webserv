
#ifndef TOML98_INCLUDE_PARSER_HPP
#define TOML98_INCLUDE_PARSER_HPP

#include <sys/types.h>

#include <string>
#include <vector>

#include "Lexer.hpp"
#include "Value.hpp"

namespace toml98 {

// enum ParserState {
// ParserStateNormal,
//
// ParserStateSuperKeyArray,
// ParserStateSuperKeyTable,
// ParserStateKey,
//
// ParserStateInlineArray,
// ParserStateInlineTable,
//
// ParserStateValue,
// };

class Parser {
 public:
  struct KeyPath {
    std::vector<std::string> path;
  };

  struct Statement {
    KeyPath path;
    bool inlined;
  };

  Value parse(std::vector<Token> tokens);

 private:
  Value* _document;
  std::vector<Token> _data;
  // std::stack<ParserState> _stack;
  // std::vector<PathPart> _root;
  u_int64_t _pos;
  // bool _isLastEqual;
  // TokenType _lastTokType;

  void readKeyPair(std::vector<PathPart> root);
  void readValue(std::vector<PathPart> root, const PathPart& key);

  void readBody(const std::vector<PathPart>& root);
  void readTable(const std::vector<PathPart>& root);
  void readArray(const std::vector<PathPart>& root);
  void readSuperKey(bool isArray);

  std::vector<PathPart> readKeyPath(TokenType superMario);

  std::vector<PathPart> _buffer;

  void pop();
  void pop(u_int64_t amount);
  const Token& consume(const std::string& msg);

  const Token& peek() const;
  const Token& peek(u_int64_t offset) const;
  bool canPeek();

  void skipWhitespace();

  bool _isWordDigit(const Token& tok);
  bool _isWordFloat(const Token& tok);
  bool _isWordBoolean(const Token& tok);

  int64_t readInteger(const Token& tok);
  double readFloat(const Token& tok);
  bool readBoolean(const Token& tok);
};

}  // namespace toml98

#endif
