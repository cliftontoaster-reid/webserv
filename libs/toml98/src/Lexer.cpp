#include "Lexer.hpp"

#include <sys/types.h>

#include <cstdlib>
#include <stack>
#include <stdexcept>
#include <string>

#include "utils.hpp"

static inline u_int64_t getSpecialLenght(char code) {
  switch (code) {
    case 'b':
    case 't':
    case 'n':
    case 'f':
    case 'r':
    case 'e':
    case '"':
    case '\\':
      return UTF8_ESCLEN_SPECIAL;
    case 'x':
      return UTF8_ESCLEN_X;
    case 'u':
      return UTF8_ESCLEN_U;
    case 'U':
      return UTF8_ESCLEN_UU;
    default:
      throw std::runtime_error("Unexpected special character.");
  }
}

namespace toml98 {

Token::Token() : type(TokenWord) {}
Token::Token(TokenType type, const std::string& value)
    : type(type), value(value) {}
Token::Token(const Token& other) : type(other.type), value(other.value) {}
Token& Token::operator=(const Token& other) {
  if (this != &other) {
    type = other.type;
    value = other.value;
  }
  return *this;
}
Token::~Token() {}

Lexer::Lexer() : _pos(0), _isLastEqual(false) { _stack.push(LexerNormal); }
Lexer::Lexer(const std::string& str)
    : _source(str), _pos(0), _isLastEqual(false) {
  _stack.push(LexerNormal);
}
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

void Lexer::pop() { pop(1); }
void Lexer::pop(u_int64_t amount) {
  if (_pos + amount > _source.length()) {
    throw std::runtime_error("Early end of file.");
  }
  _pos += amount;
}
char Lexer::peek() { return peek(0); }
char Lexer::peek(u_int64_t offset) {
  if ((_pos + offset) >= _source.length()) {
    throw std::runtime_error("Early end of file.");
  }
  return _source.at(_pos + offset);
}
std::string Lexer::peek(u_int64_t offset, u_int64_t size) {
  if ((_pos + offset + size) >= _source.length()) {
    throw std::runtime_error("Early end of file.");
  }

  return _source.substr(_pos + offset, size);
}

bool Lexer::canPeek() { return (_pos < _source.length()); }
bool Lexer::canPeek(char expect) { return canPeek(expect, 0); }
bool Lexer::canPeek(char expect, u_int64_t offset) {
  if ((_pos + offset) >= _source.length()) {
    return false;
  }
  return (_source.at(_pos + offset) == expect);
}
bool Lexer::canPeekAt(u_int64_t offset) {
  return (_pos + offset < _source.length());
}
u_int64_t Lexer::remaining() { return _source.length() - _pos; }

void Lexer::handleEscapeSequence(std::string& output) {
  if (!canPeek()) {
    throw std::runtime_error("Early End Of File.");
  }

  u_int64_t len = getSpecialLenght(peek());
  if (!canPeekAt(len)) {
    throw std::runtime_error("Early End Of File.");
  }

  if (len == 1) {
    output.push_back(getSpecial(peek()));
    pop();
  } else {
    output.append(getUnicode(peek(1, len - 1)));
    pop(len);
  }
}

bool Lexer::handleQuoteSequence(std::string& output, char quote) {
  std::size_t quoteCount = 1;
  while (canPeek(quote)) {
    quoteCount++;
    pop();
  }

  if (quoteCount >= 3) {
    if (quoteCount > 3) {
      output.append(quoteCount - 3, quote);
    }
    return true;
  }

  output.append(quoteCount, quote);
  return false;
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
