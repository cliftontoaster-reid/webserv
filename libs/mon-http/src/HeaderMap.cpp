#include "HeaderMap.hpp"

#include <cctype>
#include <string>

#include "HashMap.hpp"

namespace mon_http {

HeaderMap::HeaderMap() {}
HeaderMap::HeaderMap(const HeaderMap& other) {
  HashMap<std::string, std::string> copy(other._store);

  _store = copy;
}
HeaderMap& HeaderMap::operator=(const HeaderMap& other) {
  if (this != &other) {
    HashMap<std::string, std::string> copy(other._store);

    _store = copy;
  }
  return *this;
}
HeaderMap::~HeaderMap() {}

std::string& HeaderMap::at(const std::string& key) {
  return _store.at(_normalize(key));
}
std::string& HeaderMap::operator[](const std::string& key) {
  return _store[_normalize(key)];
}

void HeaderMap::clear() { _store.clear(); }

void HeaderMap::insert(const std::string& key, const std::string& value) {
  _store.insert(_normalize(key), value);
}

void HeaderMap::remove(const std::string& key) {
  _store.remove(_normalize(key));
}

std::string HeaderMap::_normalize(const std::string& key) {
  std::string copy(key);

  for (size_t i = 0; i < copy.length(); i++) {
    copy[i] = static_cast<char>(std::tolower(copy[i]));
  }

  return copy;
}

}  // namespace mon_http