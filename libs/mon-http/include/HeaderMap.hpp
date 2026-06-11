
#ifndef MON_HTTP_INCLUDE_HEADERMAP_HTTP
#define MON_HTTP_INCLUDE_HEADERMAP_HTTP

#include <string>

#include "HashMap.hpp"

namespace mon_http {

class HeaderMap {
 public:
  HeaderMap();
  HeaderMap(const HeaderMap& other);
  HeaderMap& operator=(const HeaderMap& other);
  ~HeaderMap();

  std::string& at(const std::string& key);
  std::string& operator[](const std::string& key);

  bool contains(const std::string& key);

  void clear();

  void insert(const std::string& key, const std::string& value);
  void remove(const std::string& key);

  template <typename F>
  void iter(F func);

 private:
  static std::string _normalize(std::string key);

  HashMap<std::string, std::string> _store;
};

}  // namespace mon_http

#include "HeaderMap.tpp"

#endif