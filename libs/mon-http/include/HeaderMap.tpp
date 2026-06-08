
#include <cctype>
#include <string>

namespace mon_http {

inline HeaderMap::HeaderMap() {}

inline HeaderMap::HeaderMap(const HeaderMap& other) {
  HashMap<std::string, std::string> copy(other._store);
  _store = copy;
}

inline HeaderMap& HeaderMap::operator=(const HeaderMap& other) {
  if (this != &other) {
    HashMap<std::string, std::string> copy(other._store);
    _store = copy;
  }
  return *this;
}

inline HeaderMap::~HeaderMap() {}

inline std::string& HeaderMap::at(const std::string& key) {
  return _store.at(_normalize(key));
}

inline std::string& HeaderMap::operator[](const std::string& key) {
  return _store[_normalize(key)];
}

inline void HeaderMap::clear() { _store.clear(); }

inline void HeaderMap::insert(const std::string& key, const std::string& value) {
  _store.insert(_normalize(key), value);
}

inline void HeaderMap::remove(const std::string& key) {
  _store.remove(_normalize(key));
}

template <typename F>
inline void HeaderMap::iter(F func) {
  _store.iter(func);
}

inline std::string HeaderMap::_normalize(const std::string& key) {
  std::string copy(key);
  for (size_t i = 0; i < copy.length(); i++) {
    copy[i] = static_cast<char>(std::tolower(copy[i]));
  }
  return copy;
}

}  // namespace mon_http
