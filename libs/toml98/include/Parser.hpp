
#ifndef TOML98_INCLUDE_PARSER_HPP
#define TOML98_INCLUDE_PARSER_HPP

#include <string>
#include <vector>

#include "Lexer.hpp"
#include "Value.hpp"

namespace toml98 {

enum ParserState {
  ParserStateNormal,

  ParserStateSuperKey,
  ParserStateKey,

  ParserStateInlineArray,
  ParserStateInlineTable,

  ParserStateValue,
};

class Parser {
 public:
  struct KeyPath {
    std::vector<std::string> path;
  };

  struct Statement {
    KeyPath path;
    bool inlined;
  };

  void handle_normal();
  void handle_super_key();
  void handle_key();
  void handle_inline_array();
  void handle_inline_table();
  void handle_value();

 private:
  Value* _document;
  std::vector<Token> _data;
  std::stack<ParserState> _stack;
  std::string _root;
  u_int64_t _pos;
  bool _isLastEqual;
  ParserState _lastState;

  void pop();
  void pop(u_int64_t amount);

  const Token& peek() const;
  const Token& peek(u_int64_t offset) const;

  bool _isWordDigit();
  bool _isWordFloat();
};

}  // namespace toml98

#endif