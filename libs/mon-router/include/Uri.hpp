
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
  const std::string& scheme() const;
  bool hasScheme() const;
  std::string& authority();
  const std::string& authority() const;
  bool hasAuthority() const;
  std::string& path();
  const std::string& path() const;
  bool hasPath() const;
  mon_http::HashMap<std::string, std::string>& query();
  const mon_http::HashMap<std::string, std::string>& query() const;
  bool hasQuery() const;
  std::string& fragment();
  const std::string& fragment() const;
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