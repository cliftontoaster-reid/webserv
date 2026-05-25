#ifndef TOML98_TESTS_INCLUDE_STUPID_LEXER_HPP
#define TOML98_TESTS_INCLUDE_STUPID_LEXER_HPP

#include <stack>
#include <string>

#include "Lexer.hpp"

namespace toml98 {

class StupidLexer : public Lexer {
 public:
  StupidLexer() {};
  explicit StupidLexer(const std::string& str) : Lexer(str) {}

  std::stack<LexerState>& stupid_stack() { return _stack; }
  std::string& stupid_source() { return _source; }
  std::string& stupid_buffer() { return _buffer; }
  u_int64_t& stupid_pos() { return _pos; }
  bool& stupid_is_last_equal() { return _isLastEqual; }

  void stupid_push_state(LexerState state) { _stack.push(state); }

  Token* stupid_handle_normal() { return handle_normal(); }
  Token* stupid_handle_word() { return handle_word(); }
  Token* stupid_handle_string() { return handle_string(); }
  Token* stupid_handle_string_double() { return handle_string_double(); }
  Token* stupid_handle_string_multiline() { return handle_string_multiline(); }
  Token* stupid_handle_string_double_multiline() {
    return handle_string_double_multiline();
  }
  Token* stupid_handle_table_key() { return handle_table_key(); }
  Token* stupid_handle_array_key() { return handle_array_key(); }
  Token* stupid_handle_comments() { return handle_comments(); }
  Token* stupid_handle_inline_array() { return handle_inline_array(); }
  Token* stupid_handle_inline_table() { return handle_inline_table(); }
  Token* stupid_handle_whitespace() { return handle_whitespace(); }

  void stupid_handle_escape_sequence(std::string& output) {
    handleEscapeSequence(output);
  }
  bool stupid_handle_quote_sequence(std::string& output, char quote) {
    return handleQuoteSequence(output, quote);
  }

  void stupid_pop() { pop(); }
  void stupid_pop(u_int64_t amount) { pop(amount); }

  char stupid_peek() { return peek(); }
  char stupid_peek(u_int64_t offset) { return peek(offset); }
  std::string stupid_peek(u_int64_t offset, u_int64_t size) {
    return peek(offset, size);
  }

  bool stupid_can_peek() { return canPeek(); }
  bool stupid_can_peek(char expect) { return canPeek(expect); }
  bool stupid_can_peek(char expect, u_int64_t offset) {
    return canPeek(expect, offset);
  }
  bool stupid_can_peek_at(u_int64_t offset) { return canPeekAt(offset); }
  u_int64_t stupid_remaining() { return remaining(); }
};

}  // namespace toml98

#endif
