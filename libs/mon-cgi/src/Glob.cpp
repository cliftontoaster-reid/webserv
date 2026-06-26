#include "Glob.hpp"

#include <string>

namespace {

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
bool matchPosixClass(const std::string& str, char c) {
  if (str == "[:digit:]") {
    return (c >= '0' && c <= '9');
  }
  if (str == "[:alpha:]") {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  }
  if (str == "[:alnum:]") {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z');
  }
  if (str == "[:lower:]") {
    return (c >= 'a' && c <= 'z');
  }
  if (str == "[:upper:]") {
    return (c >= 'A' && c <= 'Z');
  }
  if (str == "[:space:]") {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' ||
            c == '\f');
  }
  if (str == "[:blank:]") {
    return (c == ' ' || c == '\t');
  }
  if (str == "[:xdigit:]") {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
  }
  if (str == "[:print:]") {
    return (c >= ' ' && c <= '~');
  }
  if (str == "[:graph:]") {
    return (c >= '!' && c <= '~');
  }
  if (str == "[:punct:]") {
    return (c >= '!' && c <= '~') && (c < '0' || c > '9') &&
           (c < 'a' || c > 'z') && (c < 'A' || c > 'Z');
  }
  if (str == "[:cntrl:]") {
    return (c >= 0 && c <= 31) || (c == 127);
  }

  return false;
}

}  // namespace

namespace mon_cgi {
namespace {

void parseBracketParts(const std::string& pattern, size_t& i, bool neg,
                       std::vector<GlobMatch::BracketPart>& parts) {
  while (i < pattern.size() && pattern[i] != ']') {
    GlobMatch::BracketPart part;

    if (pattern[i] == '[' && i + 1 < pattern.size() && pattern[i + 1] == ':') {
      size_t end = pattern.find(":]", i + 2);
      if (end != std::string::npos) {
        part.value = pattern.substr(i, end - i + 1);
        part.type = neg ? GlobMatch::BracketPart::DisallowedGroup
                        : GlobMatch::BracketPart::AllowedGroup;
        i = end + 2;
        parts.push_back(part);
        continue;
      }
    }

    if (i + 2 < pattern.size() && pattern[i + 1] == '-' &&
        pattern[i + 2] != ']') {
      part.value = std::string(1, pattern[i]) + pattern[i + 2];
      part.type = neg ? GlobMatch::BracketPart::DisallowedRange
                      : GlobMatch::BracketPart::AllowedRange;
      i += 3;
    } else {
      part.value = std::string(1, pattern[i]);
      part.type = neg ? GlobMatch::BracketPart::Disallowed
                      : GlobMatch::BracketPart::Allowed;
      ++i;
    }

    parts.push_back(part);
  }
}

}  // namespace

// -------------------------------------------------------------------
// GlobMatch constructors
// -------------------------------------------------------------------

GlobMatch::GlobMatch() : _type(Any), _c(0), _next1(0), _next2(0) {}

GlobMatch::GlobMatch(char c, Type type)
    : _type(type), _c(c), _next1(0), _next2(0) {}

GlobMatch::GlobMatch(const std::string& str, Type type)
    : _type(type), _c(0), _next1(0), _next2(0) {
  (void)str;
}

GlobMatch::GlobMatch(Type type, size_t next1, size_t next2)
    : _type(type), _c(0), _next1(next1), _next2(next2) {}

GlobMatch::GlobMatch(Type type, char c, size_t next1, size_t next2)
    : _type(type), _c(c), _next1(next1), _next2(next2) {}

GlobMatch::GlobMatch(Type type, const std::vector<BracketPart>& parts,
                     size_t next1, size_t next2)
    : _type(type), _c(0), _next1(next1), _next2(next2), _bracket(parts) {}

GlobMatch::GlobMatch(const GlobMatch& other)
    : _type(other._type),
      _c(other._c),
      _next1(other._next1),
      _next2(other._next2),
      _bracket(other._bracket) {}

GlobMatch& GlobMatch::operator=(const GlobMatch& other) {
  if (this != &other) {
    _type = other._type;
    _c = other._c;
    _next1 = other._next1;
    _next2 = other._next2;
    _bracket = other._bracket;
  }
  return *this;
}

const std::vector<GlobMatch::BracketPart>& GlobMatch::getBracket() const {
  return _bracket;
}

char GlobMatch::getChar() const { return _c; }

GlobMatch::Type GlobMatch::getType() const { return _type; }

size_t GlobMatch::getNext1() const { return _next1; }
size_t GlobMatch::getNext2() const { return _next2; }
void GlobMatch::setNext1(size_t n) { _next1 = n; }
void GlobMatch::setNext2(size_t n) { _next2 = n; }

bool Glob::compile(const std::string& pattern) {
  _program.clear();

  std::vector<size_t> pending;

  for (size_t i = 0; i < pattern.size(); ++i) {
    size_t pos = _program.size();

    if (pattern[i] == '*') {
      size_t split = pos;
      _program.push_back(GlobMatch(GlobMatch::Split, pos + 1, 0));
      _program.push_back(GlobMatch(GlobMatch::Any, split, 0));
      pending.push_back(split);
    } else if (pattern[i] == '?') {
      _program.push_back(GlobMatch(GlobMatch::Any, 0, 0));
      for (size_t j = 0; j < pending.size(); ++j) {
        _program[pending[j]].setNext2(pos);
      }
      pending.clear();
    } else if (pattern[i] == '\\' && i + 1 < pattern.size()) {
      ++i;
      _program.push_back(GlobMatch(GlobMatch::Literal, pattern[i], 0, 0));
      for (size_t j = 0; j < pending.size(); ++j) {
        _program[pending[j]].setNext2(pos);
      }
      pending.clear();
    } else if (pattern[i] == '[') {
      ++i;
      bool neg =
          (i < pattern.size() && (pattern[i] == '!' || pattern[i] == '^'));
      if (neg) {
        ++i;
      }

      std::vector<GlobMatch::BracketPart> parts;
      parseBracketParts(pattern, i, neg, parts);

      if (i < pattern.size() && pattern[i] == ']') ++i;

      _program.push_back(GlobMatch(GlobMatch::Bracket, parts, 0, 0));
      for (size_t j = 0; j < pending.size(); ++j) {
        _program[pending[j]].setNext2(pos);
      }
      pending.clear();
    } else {
      _program.push_back(GlobMatch(GlobMatch::Literal, pattern[i], 0, 0));
      for (size_t j = 0; j < pending.size(); ++j) {
        _program[pending[j]].setNext2(pos);
      }
      pending.clear();
    }
  }

  size_t match = _program.size();
  _program.push_back(GlobMatch(GlobMatch::Match, 0, 0));

  for (size_t j = 0; j < pending.size(); ++j) {
    _program[pending[j]].setNext2(match);
  }

  return true;
}

void Glob::addState(std::vector<size_t>& set, std::vector<bool>& visited,
                    size_t idx) const {
  if (visited[idx]) {
    return;
  }
  visited[idx] = true;

  if (_program[idx].getType() == GlobMatch::Split) {
    addState(set, visited, _program[idx].getNext1());
    addState(set, visited, _program[idx].getNext2());
  } else {
    set.push_back(idx);
  }
}

bool Glob::matches(const std::string& str) {
  if (_program.empty()) {
    return false;
  }

  std::vector<bool> visited(_program.size());
  std::vector<size_t> cur;
  addState(cur, visited, 0);

  for (size_t i = 0; i < str.size(); ++i) {
    char c = str[i];
    std::vector<size_t> next;
    std::fill(visited.begin(), visited.end(), false);

    for (size_t j = 0; j < cur.size(); ++j) {
      const GlobMatch& inst = _program[cur[j]];

      switch (inst.getType()) {
        case GlobMatch::Literal:
          if (match_litteral(inst, c)) {
            addState(next, visited, inst.getNext1());
          }
          break;
        case GlobMatch::Any:
          addState(next, visited, inst.getNext1());
          break;
        case GlobMatch::Bracket:
          if (match_bracket(inst, c)) {
            addState(next, visited, inst.getNext1());
          }
          break;
        default:
          break;
      }
    }

    cur.swap(next);
    if (cur.empty()) {
      return false;
    }
  }

  for (size_t j = 0; j < cur.size(); ++j) {
    if (_program[cur[j]].getType() == GlobMatch::Match) {
      return true;
    }
  }
  return false;
}

bool Glob::match_litteral(const GlobMatch& state, char c) const {
  return state.getChar() == c;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
bool Glob::match_bracket(const GlobMatch& state, char c) const {
  const std::vector<GlobMatch::BracketPart>& parts = state.getBracket();

  for (size_t i = 0; i < parts.size(); ++i) {
    const GlobMatch::BracketPart& part = parts[i];

    switch (part.type) {
      case GlobMatch::BracketPart::Allowed:
        if (!part.value.empty() && c == part.value[0]) {
          return true;
        }
        break;

      case GlobMatch::BracketPart::Disallowed:
        if (!part.value.empty() && c == part.value[0]) {
          return false;
        }
        break;

      case GlobMatch::BracketPart::AllowedRange:
        if (part.value.length() == 2) {
          if (c >= part.value[0] && c <= part.value[1]) {
            return true;
          }
        }
        break;

      case GlobMatch::BracketPart::DisallowedRange:
        if (part.value.length() == 2) {
          if (c >= part.value[0] && c <= part.value[1]) {
            return false;
          }
        }
        break;

      case GlobMatch::BracketPart::AllowedGroup:
        if (matchPosixClass(part.value, c)) {
          return true;
        }
        break;

      case GlobMatch::BracketPart::DisallowedGroup:
        if (matchPosixClass(part.value, c)) {
          return false;
        }
        break;
    }
  }

  if (!parts.empty()) {
    GlobMatch::BracketPart::Type first_type = parts[0].type;
    if (first_type == GlobMatch::BracketPart::Disallowed ||
        first_type == GlobMatch::BracketPart::DisallowedRange ||
        first_type == GlobMatch::BracketPart::DisallowedGroup) {
      return true;
    }
  }

  return false;
}
}  // namespace mon_cgi