#include "AHttpRequest.hpp"

#include <string>

#include "HttpVersion.hpp"

namespace mon_http {

struct HeaderPrinter {
  std::ostream& oStr;  // NOLINT
  explicit HeaderPrinter(std::ostream& oSt) : oStr(oSt) {}

  // The function call operator that matches your expected signature
  void operator()(const std::string& key, const std::string& value) const {
    oStr << "  - " << key << " : " << value << '\n';
  }
};

// Assuming your header container's iter method accepts a function object
std::ostream& operator<<(std::ostream& oStr, AHttpRequest& use) {
  std::string ver = use.version().toString();
  std::string method = use.method().toString();
  const std::string& path = use.path();

  oStr << "Http request : " << method << " " << path << " " << ver << '\n';
  use.headers().iter(HeaderPrinter(oStr));

  return oStr;
}

}  // namespace mon_http