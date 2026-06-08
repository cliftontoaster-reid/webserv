
#ifndef TOML98_INCLUDE_PARSER_HPP
#define TOML98_INCLUDE_PARSER_HPP

#include <sys/types.h>

#include <map>
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
#ifdef TOML98_TESTS
  friend class StupidParser;
#endif

 public:
  Parser() : _document(NULL), _pos(0) {}
  ~Parser() {
    if (_document != NULL) {
      delete _document;
    }
  }
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
  std::map<std::vector<PathPart>, int64_t> _depthMap;

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
  bool canPeek(u_int64_t offset);

  void skipWhitespace();

  bool isWordFloat(const Token& tok);
  static bool isWordDigit(const Token& tok);
  static bool isWordBoolean(const Token& tok);

  double readFloat(const Token& tok);
  static int64_t readInteger(const Token& tok);
  static bool readBoolean(const Token& tok);

  void insertOrDie(const std::vector<PathPart>& path, const Value& value);
};

}  // namespace toml98

#endif
