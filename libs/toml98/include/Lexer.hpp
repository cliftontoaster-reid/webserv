
#ifndef TOML98_INCLUDE_LEXER_HPP
#define TOML98_INCLUDE_LEXER_HPP

#include <sys/types.h>

#include <stack>
#include <string>

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
  Lexer(const std::string& s);
  Lexer(const Lexer& other);
  Lexer& operator=(const Lexer& other);
  ~Lexer();

  Token* run();
  void push(const std::basic_string<char>& s);

 private:
  std::stack<LexerState> _stack;
  std::string _source;
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
};

char getSpecial(char c);

std::string getUnicode(const std::string& s);

}  // namespace toml98

#endif