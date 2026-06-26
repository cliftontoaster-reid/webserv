#ifndef MON_CGI_INCLUDE_GLOB_HPP
#define MON_CGI_INCLUDE_GLOB_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace mon_cgi {

enum State {
  Match,
  Any,
  Split,
  Literal,
  Bracket,
};

class GlobMatch {
 public:
  enum Type {
    Any = 1,
    Split,
    Literal,
    Bracket,
    Match,
  };

  struct BracketPart {
    enum Type {
      Allowed,
      Disallowed,
      AllowedRange,
      DisallowedRange,
      AllowedGroup,
      DisallowedGroup,
    };

    Type type;
    std::string value;
  };

  GlobMatch();
  explicit GlobMatch(char c, Type type);
  explicit GlobMatch(const std::string& str, Type type);
  GlobMatch(Type type, size_t next1, size_t next2);
  GlobMatch(Type type, char c, size_t next1, size_t next2);
  GlobMatch(Type type, const std::vector<BracketPart>& parts, size_t next1,
            size_t next2);
  GlobMatch(const GlobMatch& other);
  GlobMatch& operator=(const GlobMatch& other);

  const std::vector<BracketPart>& getBracket() const;
  char getChar() const;

  Type getType() const;
  size_t getNext1() const;
  size_t getNext2() const;
  void setNext1(size_t n);
  void setNext2(size_t n);

 private:
  Type _type;
  char _c;
  size_t _next1;
  size_t _next2;
  std::vector<BracketPart> _bracket;
};

class Glob {
 public:
  bool compile(const std::string& pattern);
  bool matches(const std::string& str);

 private:
  std::vector<GlobMatch> _program;

  bool match_litteral(const GlobMatch& state, char c) const;
  bool match_bracket(const GlobMatch& state, char c) const;
  void addState(std::vector<size_t>& set, std::vector<bool>& visited,
                size_t idx) const;
};

}  // namespace mon_cgi

#endif
