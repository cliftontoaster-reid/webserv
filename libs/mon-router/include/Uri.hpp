
#ifndef MON_ROUTER_INCLUDE_URI_HPP
#define MON_ROUTER_INCLUDE_URI_HPP

#include <string>

#include "HashMap.hpp"

namespace mon_router {

class Uri {
 public:
  Uri();
  explicit Uri(const std::string& uri);
  Uri(const Uri& other);
  Uri& operator=(const Uri& other);

  std::string& scheme();
  bool hasScheme() const;
  std::string& authority();
  bool hasAuthority() const;
  std::string& path();
  bool hasPath() const;
  mon_http::HashMap<std::string, std::string>& query();
  bool hasQuery() const;
  std::string& fragment();
  bool hasFragment() const;

 private:
  std::string _scheme;
  bool _has_scheme;
  std::string _authority;
  bool _has_authority;
  std::string _path;
  bool _has_path;
  mon_http::HashMap<std::string, std::string> _query;
  bool _has_query;
  std::string _fragment;
  bool _has_fragment;

  void _parseURI(const std::string& uri);
};

}  // namespace mon_router

#endif