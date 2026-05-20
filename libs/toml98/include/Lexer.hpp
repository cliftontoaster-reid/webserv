
#ifndef TOML98_INCLUDE_LEXER_HPP
#define TOML98_INCLUDE_LEXER_HPP

#include <sys/types.h>

#include <stack>
#include <string>

// UTF-8 Payload Extraction Masks and Shifts
enum Utf8Payload {
  UTF8_CONTINUATION_MASK = 0x3F,  // 00111111 (Extracts 6 bits of payload data)
  UTF8_SHIFT_6BITS = 6,           // Shifts past 1 continuation byte payload
  UTF8_SHIFT_12BITS = 12,         // Shifts past 2 continuation bytes payload
  UTF8_SHIFT_18BITS = 18          // Shifts past 3 continuation bytes payload
};

// Maximum Unicode Codepoint Thresholds for UTF-8 Byte Lengths
enum Utf8Max {
  UTF8_MAX_1BYTE = 0x7F,     // U+0000   to U+007F
  UTF8_MAX_2BYTE = 0x7FF,    // U+0080   to U+07FF
  UTF8_MAX_3BYTE = 0xFFFF,   // U+0800   to U+FFFF
  UTF8_MAX_4BYTE = 0x10FFFF  // U+100000 to U+10FFFF
};

// Leading and Continuation Byte Bitwise Prefixes
enum Utf8Prefix {
  UTF8_LEAD_2BYTE = 0xC0,        // 110xxxxx (Starts a 2-byte sequence)
  UTF8_LEAD_3BYTE = 0xE0,        // 1110xxxx (Starts a 3-byte sequence)
  UTF8_LEAD_4BYTE = 0xF0,        // 11110xxx (Starts a 4-byte sequence)
  UTF8_CONTINUATION_LEAD = 0x80  // 10xxxxxx (Marks a trailing payload byte)
};

namespace toml98 {

enum TokenType {
  TokenWord,
  TokenString,
  TokenMultiString,

  // Table keys
  TokenTableKeyStart,
  TokenTableKeyEnd,
  // Array keys
  TokenArrayKeyStart,
  TokenArrayKeyEnd,
  // Inline arrays
  TokenArrayStart,
  TokenArrayEnd,
  // Inline tables
  TokenTableStart,
  TokenTableEnd,

  TokenEqual,  // '=' or ':' based on context
  TokenDelimiter,
  TokenDot,
};

struct Token {
  Token();
  Token(TokenType type, const std::string& value);
  Token(const Token& other);
  Token& operator=(const Token& other);
  ~Token();

  TokenType type;
  std::string value;
};

enum LexerState {
  LexerNormal,
  LexerWord,
  LexerString,                 //   ' ... '
  LexerStringDouble,           //   " ... "
  LexerStringMultiLine,        // ''' ... '''
  LexerStringDoubleMultiLine,  // """ ... """
  LexerTableKey,               //   [ ... ]
  LexerArrayKey,               //  [[ ... ]]
  LexerComments,               //   # ...
  LexerInlineArray,            // = [ ... ]
  LexerInlineTable,            //   { ... }
  WhiteSpace,
};

class Lexer {
 public:
  Lexer();
  explicit Lexer(const std::string& str);
  Lexer(const Lexer& other);
  Lexer& operator=(const Lexer& other);
  ~Lexer();

  Token* run();
  void push(const std::basic_string<char>& str);

 private:
  std::stack<LexerState> _stack;
  std::string _source;
  std::string _buffer;
  u_int64_t _pos;

  Token* handle_normal();
  Token* handle_word();
  Token* handle_string();
  Token* handle_string_double();
  Token* handle_string_multiline();
  Token* handle_string_double_multiline();
  Token* handle_table_key();
  Token* handle_array_key();
  Token* handle_comments();
  Token* handle_inline_array();
  Token* handle_inline_table();
  Token* handle_whitespace();

  char pop();
  char peek();
};

char getSpecial(char code);

std::string getUnicode(const std::string& str);

}  // namespace toml98

#endif