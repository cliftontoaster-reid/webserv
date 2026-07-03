#include "Uri.hpp"

#include <sys/types.h>

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Detect.hpp"
#include "HashMap.hpp"

namespace {

inline unsigned char hexVal(char c) {
  if ('0' <= c && c <= '9') {
    return c - '0';
  }
  if ('A' <= c && c <= 'F') {
    return c - 'A' + 10;
  }
  if ('a' <= c && c <= 'f') {
    return c - 'a' + 10;
  }
  throw std::invalid_argument("non-hex digit");
}

std::string uriDecode(const std::string& src) {
  std::string out;
  out.reserve(src.size());

  for (std::size_t i = 0; i < src.size(); ++i) {
    if (src[i] == '%' && i + 2 < src.size() &&
        (std::isxdigit(static_cast<unsigned char>(src[i + 1])) != 0) &&
        (std::isxdigit(static_cast<unsigned char>(src[i + 2])) != 0)) {
      try {
        unsigned char high = hexVal(src[i + 1]);
        unsigned char low = hexVal(src[i + 2]);
#if IS_LITTLE_ENDIAN
        out.push_back(static_cast<char>((low << 4) | high));
#else
        out.push_back(static_cast<char>((high << 4) | low));
#endif
        i += 2;
      } catch (const std::invalid_argument&) {
        out.push_back('%');
      }
    } else {
      out.push_back(src[i]);
    }
  }
  return out;
}

inline std::vector<std::string> splitByDelimiter(const std::string& str,
                                                 size_t start, size_t endAt,
                                                 char delimiter) {
  std::vector<std::string> tokens;
  std::string::size_type end = str.find(delimiter, start);

  while (end != std::string::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + 1;
    end = str.find(delimiter, start);
    if (end >= endAt) {
      break;
    }
  }

  tokens.push_back(str.substr(start, std::min(endAt, str.length())));
  return tokens;
}

inline std::pair<std::string, std::string> parseKeyValuePair(
    const std::string& segment) {
  std::size_t eqPos = segment.find('=');

  if (eqPos == std::string::npos) {
    return std::make_pair(segment, "");
  }

  std::string key = uriDecode(segment.substr(0, eqPos));
  std::string value = uriDecode(segment.substr(eqPos + 1));

  return std::make_pair(key, value);
}

inline bool wholeString(const std::string& str, bool test(char)) {
  for (size_t i = 0; i < str.length(); i++) {
    if (test(str.at(i)) != 0) {
      return false;
    }
  }
  return true;
}

bool _validSchemaChar(char c) {
  if (std::isalnum(c) == 0) {
    return true;
  }

  if (c == '+' || c == '-' || c == '.') {
    return true;
  }
  return false;
}

}  // namespace

namespace mon_router {

Uri::Uri()
    : _has_scheme(false),
      _has_authority(false),
      _has_path(false),
      _has_query(false),
      _has_fragment(false) {}
Uri::Uri(const std::string& uri) { _parseURI(uri); }
Uri::Uri(const Uri& other)
    : _scheme(other._scheme),
      _has_scheme(other._has_scheme),
      _authority(other._authority),
      _has_authority(other._has_authority),
      _path(other._path),
      _has_path(other._has_path),
      _query(other._query),
      _has_query(other._has_query),
      _fragment(other._fragment),
      _has_fragment(other._has_fragment) {}
Uri& Uri::operator=(const Uri& other) {
  if (this != &other) {
    this->_scheme = other._scheme;
    this->_has_scheme = other._has_scheme;
    this->_authority = other._authority;
    this->_has_authority = other._has_authority;
    this->_path = other._path;
    this->_has_path = other._has_path;
    this->_query = other._query;
    this->_has_query = other._has_query;
    this->_fragment = other._fragment;
    this->_has_fragment = other._has_fragment;
  }
  return *this;
}

std::string& Uri::scheme() {
  if (!_has_scheme) {
    throw std::runtime_error("URI does not have a scheme");
  }
  return _scheme;
}
const std::string& Uri::scheme() const {
  if (!_has_scheme) {
    throw std::runtime_error("URI does not have a scheme");
  }
  return _scheme;
}
bool Uri::hasScheme() const { return _has_scheme; }

std::string& Uri::authority() {
  if (!_has_authority) {
    throw std::runtime_error("URI does not have an authority");
  }
  return _authority;
}
const std::string& Uri::authority() const {
  if (!_has_authority) {
    throw std::runtime_error("URI does not have an authority");
  }
  return _authority;
}
bool Uri::hasAuthority() const { return _has_authority; }

std::string& Uri::path() {
  if (!_has_path) {
    throw std::runtime_error("URI does not have a path");
  }
  return _path;
}
const std::string& Uri::path() const {
  if (!_has_path) {
    throw std::runtime_error("URI does not have a path");
  }
  return _path;
}
bool Uri::hasPath() const { return _has_path; }

mon_http::HashMap<std::string, std::string>& Uri::query() {
  if (!_has_query) {
    throw std::runtime_error("URI does not have queries");
  }
  return _query;
}
const mon_http::HashMap<std::string, std::string>& Uri::query() const {
  if (!_has_query) {
    throw std::runtime_error("URI does not have queries");
  }
  return _query;
}
bool Uri::hasQuery() const { return _has_query; }

std::string& Uri::fragment() {
  if (!_has_fragment) {
    throw std::runtime_error("URI does not have a fragment");
  }
  return _fragment;
}
const std::string& Uri::fragment() const {
  if (!_has_fragment) {
    throw std::runtime_error("URI does not have a fragment");
  }
  return _fragment;
}
bool Uri::hasFragment() const { return _has_fragment; }

void Uri::_parseURI(const std::string& uri) {
  size_t itUri = 0;
  {  // Parse scheme
    size_t scheme_end_pos = uri.find("://");
    if (scheme_end_pos != std::string::npos) {
      std::string scheme(uri.begin(), uri.begin() + scheme_end_pos);
      if (scheme.empty()) {
        throw std::runtime_error("URI: Scheme: cannot be empty and defined");
      }
      if (std::isalpha(scheme.at(0)) != 0) {
        throw std::runtime_error(
            "URI: Scheme: Scheme names consist of a sequence of characters "
            "beginning with a letter");
      }
      if (scheme.length() > 1 && wholeString(scheme, _validSchemaChar)) {
        throw std::runtime_error(
            "URI: Scheme: Scheme names consist of a sequence of characters "
            "beginning with a letter and followed by any combination of "
            "letters, digits, plus (\"+\"), period (\".\"), or hyphen (\"-\")");
      }
      this->_scheme.swap(scheme);
      this->_has_scheme = true;

      itUri += this->_scheme.length() + 3;
      if (itUri >= uri.length()) {
        throw std::runtime_error("URI: Scheme: URI cannot end with scheme");
      }
    } else {
      this->_has_scheme = false;
    }
  }
  {  // Parse Authority
    if (this->_has_scheme) {
      size_t authority_end_pos = uri.find_first_of("/?#", itUri);
      std::string authority;
      if (authority_end_pos != std::string::npos) {
        if (uri.begin() + authority_end_pos == uri.end()) {
          throw std::runtime_error("URI: Scheme: scheme is an empty string");
        }

        authority =
            std::string(uri.begin() + itUri, uri.begin() + authority_end_pos);

      } else {
        if (uri.begin() + itUri == uri.end()) {
          throw std::runtime_error("URI: Scheme: scheme is an empty string");
        }
        authority = std::string(uri.begin() + itUri, uri.end());
      }

      this->_authority.swap(authority);
      this->_has_authority = true;
      itUri = authority_end_pos;
      if (itUri >= uri.length()) {
        return;
      }
      return;
    }
    this->_has_authority = false;
  }
  {  // Parse path
    if (uri.at(itUri) == '/') {
      size_t path_end_pos = uri.find_first_of("?#", itUri);
      std::string path;
      if (path_end_pos != std::string::npos) {
        path = uri.substr(itUri, path_end_pos - itUri);
      } else {
        path = uri.substr(itUri);
      }

      this->_path.swap(path);
      this->_has_path = true;
      itUri = path_end_pos;
      if (itUri >= uri.length()) {
        return;
      }
    } else {
      this->_has_path = false;
    }
  }
  {  // Parse queries
    if (uri.at(itUri) == '?') {
      itUri += 1;
      if (itUri >= uri.length()) {
        throw std::runtime_error("URI: Queries: cannot be empty and defined");
      }

      size_t queries_end_pos = uri.find('#');
      std::vector<std::string> queries_raw =
          splitByDelimiter(uri, itUri, queries_end_pos, '&');

      mon_http::HashMap<std::string, std::string> queries;
      for (size_t i = 0; i < queries_raw.size(); i++) {
        const std::string& query_raw = queries_raw.at(i);

        std::pair<std::string, std::string> query =
            parseKeyValuePair(query_raw);

        // CHECK

        queries.insert(query.first, query.second);
      }

      this->_query = queries;
      this->_has_query = true;

      itUri = queries_end_pos;
      if (itUri >= uri.length()) {
        return;
      }
    }
  }
  {  // Parse fragment
    if (uri.at(itUri) == '#') {
      itUri += 1;
      if (itUri >= uri.length()) {
        throw std::runtime_error("URI: Fragment: cannot be empty and defined");
      }

      this->_fragment = std::string(uri.begin() + itUri, uri.end());
      this->_has_fragment = true;
    }
  }
}

}  // namespace mon_router