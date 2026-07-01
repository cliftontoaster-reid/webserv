
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

inline const std::string& HeaderMap::at(const std::string& key) const {
  return _store.at(_normalize(key));
}

inline std::string& HeaderMap::operator[](const std::string& key) {
  return _store[_normalize(key)];
}

inline const std::string& HeaderMap::operator[](const std::string& key) const {
  return _store.at(_normalize(key));
}

inline bool HeaderMap::contains(const std::string& key) const {
  return _store.contains(_normalize(key));
}

inline void HeaderMap::clear() { _store.clear(); }

inline void HeaderMap::insert(const std::string& key,
                              const std::string& value) {
  _store.insert(_normalize(key), value);
}

inline void HeaderMap::remove(const std::string& key) {
  _store.remove(_normalize(key));
}

template <typename F>
inline void HeaderMap::iter(F func) {
  _store.iter(func);
}

inline std::string HeaderMap::_normalize(std::string key) {
  for (size_t i = 0; i < key.length(); i++) {
    if (key[i] >= 'A' && key[i] <= 'Z') {
      key[i] += 'a' - 'A';
    }
  }
  return key;
}

}  // namespace mon_http
