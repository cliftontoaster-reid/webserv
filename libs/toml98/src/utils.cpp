#include "utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <string>

namespace toml98 {

int64_t intFromHex(const std::string& str) {
  std::string tmp = std::string();
  if (str.substr(0, 2) == "0x" || str.substr(0, 2) == "0X") {
    tmp = str.substr(2);
  } else {
    tmp = str;
  }
  tmp.erase(std::remove(tmp.begin(), tmp.end(), '_'), tmp.end());

  return strtoll(tmp.c_str(), NULL, HEX_BASE);
}

int64_t intFromOct(const std::string& str) {
  std::string tmp = std::string();
  if (str.substr(0, 2) == "0o") {
    tmp = str.substr(2);
  } else {
    tmp = str;
  }
  tmp.erase(std::remove(tmp.begin(), tmp.end(), '_'), tmp.end());

  return strtoll(str.c_str(), NULL, OCT_BASE);
}

int64_t intFromBin(const std::string& str) {
  std::string tmp = std::string();
  if (str.substr(0, 2) == "0b") {
    tmp = str.substr(2);
  } else {
    tmp = str;
  }
  tmp.erase(std::remove(tmp.begin(), tmp.end(), '_'), tmp.end());

  return strtoll(str.c_str(), NULL, BIN_BASE);
}

std::string tokTypeToString(TokenType tokty) {
  switch (tokty) {
    case TokenWord:
      return "TokenWord";
    case TokenString:
      return "TokenString";
    case TokenMultiString:
      return "TokenMultiString";
    case TokenTableKeyStart:
      return "TokenTableKeyStart";
    case TokenTableKeyEnd:
      return "TokenTableKeyEnd";
    case TokenArrayKeyStart:
      return "TokenArrayKeyStart";
    case TokenArrayKeyEnd:
      return "TokenArrayKeyEnd";
    case TokenArrayStart:
      return "TokenArrayStart";
    case TokenArrayEnd:
      return "TokenArrayEnd";
    case TokenTableStart:
      return "TokenTableStart";
    case TokenTableEnd:
      return "TokenTableEnd";
    case TokenEqual:
      return "TokenEqual";
    case TokenComma:
      return "TokenComma";
    case TokenDelimiter:
      return "TokenDelimiter";
    case TokenNewLine:
      return "TokenNewLine";
    case TokenDot:
      return "TokenDot";
    default:
      throw std::runtime_error("Unknown token type");
  }
}

}  // namespace toml98
