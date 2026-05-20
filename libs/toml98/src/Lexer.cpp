#include "Lexer.hpp"

#include <cstdlib>
#include <stack>
#include <stdexcept>
#include <string>

#include "Exception.hpp"

namespace toml98 {

Lexer::Lexer() : _source(""), _buffer(""), _pos(0) {}
Lexer::Lexer(const std::string& s) : _source(s), _buffer(""), _pos(0) {}
Lexer::Lexer(const Lexer& other)
    : _source(other._source), _buffer(other._buffer), _pos(other._pos) {
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
    if (_stack.size() == 0) {
      _stack.push(LexerNormal);
    }
    LexerState ty = _stack.top();

    switch (ty) {
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
      case WhiteSpace:
        ret = handle_whitespace();
        break;
    }
  }

  return ret;
}
void Lexer::push(const std::basic_string<char>& s) { _source.append(s); }

Token* Lexer::handle_normal() { TODO(); }
Token* Lexer::handle_word() { TODO(); }
Token* Lexer::handle_string() {
  if (!_buffer.empty()) {
    _buffer.clear();
    throw std::runtime_error(
        "Literal string started right with a non empty buffer.");
  }

  while (_pos < _source.length()) {
    char c = pop();

    if (c == '\n' || c == '\r') {
      _buffer.clear();
      throw std::runtime_error(
          "Unexpected new line in non multi-line literal string.");
    }

    if (c == '\'') {
      std::string s;
      s.swap(_buffer);
      return new Token(TokenDelimiter, s);
    }

    _buffer.push_back(c);
  }

  _buffer.clear();
  throw std::runtime_error("Unterminated literal string.");
}
Token* Lexer::handle_string_double() { TODO(); }
Token* Lexer::handle_string_multiline() { TODO(); }
Token* Lexer::handle_string_double_multiline() { TODO(); }
Token* Lexer::handle_table_key() { TODO(); }
Token* Lexer::handle_array_key() { TODO(); }
Token* Lexer::handle_comments() { TODO(); }
Token* Lexer::handle_inline_array() { TODO(); }
Token* Lexer::handle_inline_table() { TODO(); }
Token* Lexer::handle_whitespace() {
  char c;

  while ((c = pop()) != '\0' && c == ' ' && c == '\t') {
    _buffer.push_back(c);
  }

  if (_buffer.empty()) {
    return NULL;
  }
  std::string s;
  s.swap(_buffer);
  return new Token(TokenDelimiter, s);
}

char Lexer::pop() {
  if (_pos > _source.length()) {
    throw std::runtime_error("Cannot read after end of file.");
  }
  return _source[_pos++];
}
char Lexer::peek() {
  if (_pos > _source.length()) {
    throw std::runtime_error("Cannot read after end of file.");
  }
  return _source[_pos];
}

char getSpecial(char c) {
  switch (c) {
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

std::string getUnicode(const std::string& s) {
  unsigned long codepoint = 0;

  std::string hexPart;
  if (s.substr(0, 2) == "\\x" || s.substr(0, 2) == "\\u" ||
      s.substr(0, 2) == "\\U") {
    hexPart = s.substr(2);
  } else {
    hexPart = s;
  }

  codepoint = strtol(hexPart.c_str(), NULL, 16);

  std::string result;

  if (codepoint <= 0x7F) {
    result += static_cast<char>(codepoint);
  } else if (codepoint <= 0x7FF) {
    result += static_cast<char>(0xC0 | (codepoint >> 6));
    result += static_cast<char>(0x80 | (codepoint & 0x3F));
  } else if (codepoint <= 0xFFFF) {
    result += static_cast<char>(0xE0 | (codepoint >> 12));
    result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
    result += static_cast<char>(0x80 | (codepoint & 0x3F));
  } else if (codepoint <= 0x10FFFF) {
    result += static_cast<char>(0xF0 | (codepoint >> 18));
    result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
    result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
    result += static_cast<char>(0x80 | (codepoint & 0x3F));
  }

  return result;
}

}  // namespace toml98
