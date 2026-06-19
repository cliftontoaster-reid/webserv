#include "Form.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#define DIVIDER_LINES "\r\n\r\n"

namespace mon_http {

struct IsLineTerminator {
  bool operator()(char c) const { return c == '\r' || c == '\n'; }
};

static inline std::string::const_iterator findLine(
    const std::string::const_iterator& begin,
    const std::string::const_iterator& end) {
  return std::find_if(begin, end, IsLineTerminator());
}

static inline size_t _newPos(const std::string::const_iterator& lineEnd,
                             const std::string& data) {
  size_t termLen = 1;

  if (lineEnd + 1 != data.end()) {
    char first = *lineEnd;
    char second = *(lineEnd + 1);
    if ((first == '\r' && second == '\n') ||
        (first == '\n' && second == '\r')) {
      termLen = 2;
    }
  }

  return static_cast<size_t>(lineEnd - data.begin()) + termLen;
}

static inline std::pair<std::string, std::string> _parseHeaderLine(
    const std::string& line) {
  // ---- key -------------------------------------------------------------
  std::size_t keyPos = line.find_first_not_of(" \n\r");
  if (keyPos == std::string::npos) {
    throw std::runtime_error("Malformed HTTP request: missing header key");
  }

  std::size_t middlePos = line.find_first_of(" :", keyPos);
  if (middlePos == std::string::npos) {
    throw std::runtime_error(
        "Malformed HTTP request: header key not terminated");
  }

  std::string key = line.substr(keyPos, middlePos - keyPos);

  // ---- value -----------------------------------------------------------
  std::size_t valuePos = line.find_first_not_of(' ', middlePos + 1);
  if (valuePos == std::string::npos) {
    throw std::runtime_error("Malformed HTTP request: missing header value");
  }

  std::size_t endPos = line.find_first_of("\r\n", valuePos);
  if (endPos == std::string::npos) {
    endPos = line.size();
  }

  std::string value = line.substr(valuePos, endPos - valuePos);

  std::size_t lastNonSpace = value.find_last_not_of(" \t\f\v\n\r");
  if (lastNonSpace != std::string::npos) {
    value.erase(lastNonSpace + 1);
  }

  return std::make_pair(key, value);
}

static inline std::vector<std::pair<size_t, size_t> > _findSegments(
    const std::string& str, const std::string& delimiter) {
  std::vector<std::pair<size_t, size_t> > segments;

  if (delimiter.empty()) {
    return segments;
  }

  const std::string whitespace = " \t\n\r";
  size_t start = 0;
  size_t end = str.find(delimiter);

  while (end != std::string::npos) {
    size_t first = str.find_first_not_of(whitespace, start);

    if (first == std::string::npos || first >= end) {
      segments.push_back(std::make_pair(start, 0));
    } else {
      size_t last = str.find_last_not_of(whitespace, end - 1);
      segments.push_back(std::make_pair(first, last - first + 1));
    }

    start = end + delimiter.length();
    end = str.find(delimiter, start);
  }

  if (start <= str.length()) {
    size_t first = str.find_first_not_of(whitespace, start);
    if (first == std::string::npos || first >= str.length()) {
      segments.push_back(std::make_pair(start, 0));
    } else {
      size_t last = str.find_last_not_of(whitespace, str.length() - 1);
      segments.push_back(std::make_pair(first, last - first + 1));
    }
  }

  return segments;
}

FormData::FormData(const std::string& data) {
  size_t endHeader = data.find(DIVIDER_LINES);
  if (endHeader == std::string::npos) {
    throw std::invalid_argument("Form data needs a body");
  }

  size_t pos = 0;
  while (pos < endHeader) {
    std::string::const_iterator lineStart = data.begin() + pos;
    std::string::const_iterator lineEnd = findLine(lineStart, data.end());

    if (lineStart == lineEnd) {
      pos = _newPos(lineEnd, data);
      break;
    }
    std::string line(lineStart, lineEnd);

    std::pair<std::string, std::string> header = _parseHeaderLine(line);
    this->headers.insert(header.first, header.second);

    pos = _newPos(lineEnd, data);
  }

  if (endHeader < data.size()) {
    this->data.assign(data.begin() + endHeader + sizeof(DIVIDER_LINES) - 1,
                      data.end());
  } else {
    throw std::invalid_argument("Form data needs a body");
  }

  if (headers.contains("Content-Disposition")) {
    _parseContentDisposition(headers.at("Content-Disposition"));
  }
}

void FormData::_parseContentDisposition(const std::string& value) {
  std::vector<std::pair<size_t, size_t> > fragments = _findSegments(value, ";");

  if (fragments.size() < 2) {
    throw std::invalid_argument("Content-Disposition: Invalid number of parts");
  }

  type = DATA_TYPE_TEXT;
  for (size_t i = 1; i < fragments.size(); ++i) {
    size_t start = fragments[i].first;
    size_t len = fragments[i].second;

    if (len == 0) {
      continue;
    }

    std::string part = value.substr(start, len);
    size_t eq_pos = part.find('=');

    if (eq_pos != std::string::npos) {
      std::string key = part.substr(0, eq_pos);
      std::string val = part.substr(eq_pos + 1);

      if (val.length() >= 2 && val[0] == '"' && val[val.length() - 1] == '"') {
        val = val.substr(1, val.length() - 2);
      }

      if (key == "name") {
        name = val;
      } else if (key == "filename") {
        filename = val;
        type = DATA_TYPE_FILE;
      }
      if (headers.contains("Content-Disposition")) {
        _parseContentDisposition(headers.at("Content-Disposition"));
      }
    }
  }
}

Form::Form() {}
Form::Form(const std::string& content_type) {
  static const std::string allowed = "multipart/form-data";

  if (content_type.compare(0, allowed.length(), allowed) != 0) {
    throw std::invalid_argument(
        "POST requests only support 'multipart/form-data'");
  }

  std::vector<std::pair<size_t, size_t> > fragments =
      _findSegments(content_type, ";");

  bool found = false;
  for (size_t i = 0; i < fragments.size(); ++i) {
    size_t start = fragments[i].first;
    size_t len = fragments[i].second;
    std::string part = content_type.substr(start, len);

    size_t eq_pos = part.find("boundary=");
    if (eq_pos != std::string::npos) {
      _delimiter = part.substr(eq_pos + 9);
      found = true;
      break;
    }
  }

  if (!found) {
    throw std::invalid_argument(
        "Invalid 'multipart/form-data': missing boundary");
  }
}
Form::Form(const Form& other)
    : _delimiter(other._delimiter), _data(other._data) {}
Form& Form::operator=(const Form& other) {
  if (this != &other) {
    this->_delimiter = other._delimiter;
    this->_data = other._data;
  }
  return *this;
}

void Form::parse(const std::string& body) {
  size_t start = 0;
  size_t end = body.find(_delimiter);

  while (end != std::string::npos) {
    if (end > start) {
      std::string part = body.substr(start, end - start);
      if (!part.empty()) {
        FormData partData(part);
      }
    }

    start = end + _delimiter.length();
    end = body.find(_delimiter, start);
  }
}

}  // namespace mon_http