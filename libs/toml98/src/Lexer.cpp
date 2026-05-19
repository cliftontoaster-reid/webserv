#include "Lexer.hpp"

#include <cstdlib>
#include <string>

namespace toml98 {

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
