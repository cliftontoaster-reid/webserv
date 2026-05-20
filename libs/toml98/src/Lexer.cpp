#include "Lexer.hpp"

#include <sys/types.h>

#include <cctype>
#include <cstdlib>
#include <stack>
#include <stdexcept>
#include <string>

#include "Exception.hpp"
#include "utils.hpp"

namespace toml98 {

Lexer::Lexer() : _pos(0), _isLastEqual(false) {}
Lexer::Lexer(const std::string& str)
    : _source(str), _pos(0), _isLastEqual(false) {}
Lexer::Lexer(const Lexer& other)
    : _source(other._source),
      _buffer(other._buffer),
      _pos(other._pos),
      _isLastEqual(other._isLastEqual) {
  std::stack<LexerState> temp_stack = other._stack;
  std::stack<LexerState> reversed;

  while (!temp_stack.empty()) {
    reversed.push(temp_stack.top());
    temp_stack.pop();
  }

  while (!reversed.empty()) {
    _stack.push(reversed.top());
    reversed.pop();
  }
}
Lexer& Lexer::operator=(const Lexer& other) {
  if (this != &other) {
    while (!_stack.empty()) {
      _stack.pop();
    }

    std::stack<LexerState> temp_stack = other._stack;
    std::stack<LexerState> reversed;

    while (!temp_stack.empty()) {
      reversed.push(temp_stack.top());
      temp_stack.pop();
    }

    while (!reversed.empty()) {
      _stack.push(reversed.top());
      reversed.pop();
    }

    _source = other._source;
    _buffer = other._buffer;
    _pos = other._pos;
    _isLastEqual = other._isLastEqual;
  }
  return *this;
}
Lexer::~Lexer() {}

Token* Lexer::run() {
  Token* ret = NULL;
  _pos = 0;
  _buffer.clear();
  _stack = std::stack<LexerState>();
  _stack.push(LexerNormal);

  while (ret == NULL) {
    if (_stack.empty()) {
      _stack.push(LexerNormal);
    }
    LexerState current = _stack.top();

    switch (current) {
      case LexerNormal:
        ret = handle_normal();
        break;
      case LexerWord:
        ret = handle_word();
        break;
      case LexerString:
        ret = handle_string();
        break;
      case LexerStringDouble:
        ret = handle_string_double();
        break;
      case LexerStringMultiLine:
        ret = handle_string_multiline();
        break;
      case LexerStringDoubleMultiLine:
        ret = handle_string_double_multiline();
        break;
      case LexerTableKey:
        ret = handle_table_key();
        break;
      case LexerArrayKey:
        ret = handle_array_key();
        break;
      case LexerComments:
        ret = handle_comments();
        break;
      case LexerInlineArray:
        ret = handle_inline_array();
        break;
      case LexerInlineTable:
        ret = handle_inline_table();
        break;
      case LexerWhiteSpace:
        ret = handle_whitespace();
        break;
    }
  }

  if (ret->type == TokenEqual) {
    _isLastEqual = true;
  } else if (ret->type != TokenDelimiter) {
    _isLastEqual = false;
  }
  return ret;
}
void Lexer::push(const std::basic_string<char>& str) { _source.append(str); }

// NOLINTBEGIN(readability-convert-member-functions-to-static)
Token* Lexer::handle_word() {
  char letter = peek();
  while (std::isalnum(letter)) {
    pop();
    _buffer.push_back(letter);
    letter = peek();
  }
  std::string tmp;
  tmp.swap(_buffer);
  return new Token(TokenWord, tmp);
}
// NOLINTEND(readability-convert-member-functions-to-static)
Token* Lexer::handle_string() {
  if (!_buffer.empty()) {
    _buffer.clear();
    throw std::runtime_error(
        "Literal string started right with a non empty buffer.");
  }

  while (_pos < _source.length()) {
    char now = pop();

    if (now == '\n' || now == '\r') {
      _buffer.clear();
      throw std::runtime_error(
          "Unexpected new line in non multi-line literal string.");
    }

    if (now == '\'') {
      std::string tmp;
      tmp.swap(_buffer);
      return new Token(TokenDelimiter, tmp);
    }

    _buffer.push_back(now);
  }

  _buffer.clear();
  throw std::runtime_error("Unterminated literal string.");
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
Token* Lexer::handle_string_double() { TODO(); }
Token* Lexer::handle_string_multiline() {
  if (!_buffer.empty()) {
    _buffer.clear();
    throw std::runtime_error(
        "Literal string started right with a non empty buffer.");
  }

  if (_pos < _source.length() && _source.at(_pos) == '\n') {
    pop();
  } else if (_pos < _source.length() && _source.at(_pos) == '\r') {
    pop();
    if (_pos < _source.length() && _source.at(_pos) == '\n') {
      pop();
    }
  }

  while (_pos < _source.length()) {
    char now = pop();

    if (now == '\'') {
      std::size_t quoteCount = 1;
      while (_pos < _source.length() && pop() == '\'') {
        quoteCount++;
      }

      if (quoteCount >= 3) {
        if (quoteCount > 3) {
          _buffer.append(quoteCount - 3, '\'');
        }

        std::string tmp;
        tmp.swap(_buffer);
        return new Token(TokenDelimiter, tmp);
      }
      _buffer.append(quoteCount, '\'');
    }
  }

  _buffer.clear();
  throw std::runtime_error("Unterminated literal multiline string.");
}
// NOLINTBEGIN(readability-convert-member-functions-to-static)
Token* Lexer::handle_string_double_multiline() { TODO(); }
Token* Lexer::handle_table_key() { TODO(); }
Token* Lexer::handle_array_key() { TODO(); }
Token* Lexer::handle_comments() {
  while (peek() != '\n' && peek() != '\0') pop();
  return new Token(TokenNewLine, "\n");
}
Token* Lexer::handle_inline_array() { TODO(); }
Token* Lexer::handle_inline_table() { TODO(); }
// NOLINTEND(readability-convert-member-functions-to-static)
Token* Lexer::handle_whitespace() {
  char now = 0;

  while ((now = pop()) != '\0' && now == ' ' && now == '\t') {
    _buffer.push_back(now);
  }

  _stack.pop();
  if (_buffer.empty()) {
    return NULL;
  }
  std::string tmp;
  tmp.swap(_buffer);
  return new Token(TokenDelimiter, tmp);
}

char Lexer::pop() {
  if (_pos > _source.length()) {
    throw std::runtime_error("Cannot read after end of file.");
  }
  return _source.at(_pos);
}
char Lexer::peek() {
  if (_pos > _source.length()) {
    throw std::runtime_error("Cannot read after end of file.");
  }
  return _source.at(_pos);
}

char getSpecial(char code) {
  switch (code) {
    case 'b':
      return '\b';
    case 't':
      return '\t';
    case 'n':
      return '\n';
    case 'f':
      return '\f';
    case 'r':
      return '\r';
    case 'e':
      return '\e';
    case '"':
      return '"';
    case '\\':
      return '\\';
    default:
      throw std::invalid_argument("Unknown special character.");
  }
}

std::string getUnicode(const std::string& str) {
  u_int64_t codepoint = 0;

  std::string hexPart;
  if (str.substr(0, 2) == "\\x" || str.substr(0, 2) == "\\u" ||
      str.substr(0, 2) == "\\U") {
    hexPart = str.substr(2);
  } else {
    hexPart = str;
  }

  codepoint = strtol(hexPart.c_str(), NULL, HEX_BASE);

  std::string result;

  if (codepoint <= UTF8_MAX_1BYTE) {
    result += static_cast<char>(codepoint);
  } else if (codepoint <= UTF8_MAX_2BYTE) {
    result +=
        static_cast<char>(UTF8_LEAD_2BYTE | (codepoint >> UTF8_SHIFT_6BITS));
    result += static_cast<char>(UTF8_CONTINUATION_LEAD |
                                (codepoint & UTF8_CONTINUATION_MASK));
  } else if (codepoint <= UTF8_MAX_3BYTE) {
    result +=
        static_cast<char>(UTF8_LEAD_3BYTE | (codepoint >> UTF8_SHIFT_12BITS));
    result += static_cast<char>(
        UTF8_CONTINUATION_LEAD |
        ((codepoint >> UTF8_SHIFT_6BITS) & UTF8_CONTINUATION_MASK));
    result += static_cast<char>(UTF8_CONTINUATION_LEAD |
                                (codepoint & UTF8_CONTINUATION_MASK));
  } else if (codepoint <= UTF8_MAX_4BYTE) {
    result +=
        static_cast<char>(UTF8_LEAD_4BYTE | (codepoint >> UTF8_SHIFT_18BITS));
    result += static_cast<char>(
        UTF8_CONTINUATION_LEAD |
        ((codepoint >> UTF8_SHIFT_12BITS) & UTF8_CONTINUATION_MASK));
    result += static_cast<char>(
        UTF8_CONTINUATION_LEAD |
        ((codepoint >> UTF8_SHIFT_6BITS) & UTF8_CONTINUATION_MASK));
    result += static_cast<char>(UTF8_CONTINUATION_LEAD |
                                (codepoint & UTF8_CONTINUATION_MASK));
  }

  return result;
}

}  // namespace toml98
