#ifndef TOML98_TESTS_INCLUDE_STUPID_PARSER_HPP
#define TOML98_TESTS_INCLUDE_STUPID_PARSER_HPP

#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

#include <iostream>
#include <type_traits>
#include <vector>

#include "Lexer.hpp"
#include "Parser.hpp"

namespace toml98 {

class StupidParser : public Parser {
 public:
  StupidParser() {};
  explicit StupidParser(std::vector<Token> eeeeeeee) { _data.swap(eeeeeeee); };

  Value& stupid_document() { return *_document; }
  std::vector<Token>& stupid_data() { return _data; }
  u_int64_t& stupid_pos() { return _pos; }
  std::vector<PathPart>& stupid_buffer() { return _buffer; }

  Value stupid_parse(const std::vector<Token>& tokens) { return parse(tokens); }

  void stupid_readKeyPair(const std::vector<PathPart>& root) {
    readKeyPair(root);
  }
  void stupid_readValue(const std::vector<PathPart>& root,
                        const PathPart& key) {
    readValue(root, key);
  }
  void stupid_readBody(const std::vector<PathPart>& root) { readBody(root); }
  void stupid_readTable(const std::vector<PathPart>& root) { readTable(root); }
  void stupid_readArray(const std::vector<PathPart>& root) { readArray(root); }
  void stupid_readSuperKey(bool isArray) { readSuperKey(isArray); }
  std::vector<PathPart> stupid_readKeyPath(TokenType superMario) {
    return readKeyPath(superMario);
  }

  bool stupid_canPeek() { return canPeek(); }
  const Token& stupid_peek() { return peek(); }
  const Token& stupid_peek(u_int64_t offset) { return peek(offset); }
  void stupid_pop() { pop(); }
  void stupid_pop(u_int64_t amount) { pop(amount); }
  const Token& stupid_consume(const std::string& msg) { return consume(msg); }

  void stupid_skipWhitespace() { skipWhitespace(); }

  bool stupid_isWordBoolean(const Token& tok) { return isWordBoolean(tok); }
  bool stupid_isWordFloat(const Token& tok) { return isWordFloat(tok); }
  bool stupid_isWordDigit(const Token& tok) { return isWordDigit(tok); }

  int64_t stupid_readInteger(const Token& tok) { return readInteger(tok); }
  double stupid_readFloat(const Token& tok) { return readFloat(tok); }
  bool stupid_readBoolean(const Token& tok) { return readBoolean(tok); }
};

// --- check_entry variadic helpers ---

static inline std::vector<PathPart> makePath();
static inline std::vector<PathPart> makePath(const std::string& key);
static inline std::vector<PathPart> makePath(std::size_t idx);

template <typename... Rest>
static inline std::vector<PathPart> makePath(const std::string& key,
                                             const Rest&... rest);
template <typename... Rest>
static inline std::vector<PathPart> makePath(std::size_t idx,
                                             const Rest&... rest);

static inline std::vector<PathPart> makePath() {
  return std::vector<PathPart>();
}

static inline std::vector<PathPart> makePath(const std::string& key) {
  std::vector<PathPart> p;
  p.push_back(PathPart::makeKey(key));
  return p;
}

static inline std::vector<PathPart> makePath(std::size_t idx) {
  std::vector<PathPart> p;
  p.push_back(PathPart::makeIndex(idx));
  return p;
}

template <typename... Rest>
static inline std::vector<PathPart> makePath(const std::string& key,
                                             const Rest&... rest) {
  std::vector<PathPart> p = makePath(key);
  std::vector<PathPart> r = makePath(rest...);
  p.insert(p.end(), r.begin(), r.end());
  return p;
}

template <typename... Rest>
static inline std::vector<PathPart> makePath(std::size_t idx,
                                             const Rest&... rest) {
  std::vector<PathPart> p = makePath(idx);
  std::vector<PathPart> r = makePath(rest...);
  p.insert(p.end(), r.begin(), r.end());
  return p;
}

// --- string expected value ---

template <typename... Path>
static inline void check_entry(const char* file, int line, const Value& res,
                               const std::string& expected,
                               const Path&... path) {
  std::vector<PathPart> p = makePath(path...);

  if (!res.has(p) || *res.get(p).getString() != expected) {
    std::cout << "Error in " << file << ":" << line << '\n';
  }
  cr_expect(res.has(p));
  cr_expect_eq(*res.get(p).getString(), expected);
}

// --- int64_t expected value ---

template <typename... Path>
static inline void check_entry(const char* file, int line, const Value& res,
                               int64_t expected, const Path&... path) {
  std::vector<PathPart> p = makePath(path...);

  if (!res.has(p) || res.get(p).getInteger() != expected) {
    std::cout << "Error in " << file << ":" << line << '\n';
  }
  cr_expect(res.has(p));
  cr_expect_eq(res.get(p).getInteger(), expected);
}

// --- bool expected value ---

template <typename Expected, typename... Path>
static inline
    typename std::enable_if<std::is_same<Expected, bool>::value, void>::type
    check_entry(const char* file, int line, const Value& res,
                const Expected& expected, const Path&... path) {
  std::vector<PathPart> p = makePath(path...);

  if (!res.has(p) || res.get(p).getBoolean() != expected) {
    std::cout << "Error in " << file << ":" << line << '\n';
  }
  cr_expect(res.has(p));
  cr_expect_eq(res.get(p).getBoolean(), expected);
}

// --- double expected value ---

template <typename Expected, typename... Path>
static inline
    typename std::enable_if<std::is_same<Expected, double>::value, void>::type
    check_entry(const char* file, int line, const Value& res,
                const Expected& expected, const Path&... path) {
  std::vector<PathPart> p = makePath(path...);

  if (!res.has(p) || res.get(p).getFloat() != expected) {
    std::cout << "Error in " << file << ":" << line << '\n';
  }
  cr_expect(res.has(p));
  cr_expect_float_eq(res.get(p).getFloat(), expected, 0.0001, "%s:%d", file,
                     line);
}

}  // namespace toml98

#endif
