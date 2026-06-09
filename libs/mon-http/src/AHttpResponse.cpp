#include "AHttpResponse.hpp"

#include <string>
#include <vector>

namespace mon_http {

struct HeaderPrinter {
  std::ostream& oStr;
  explicit HeaderPrinter(std::ostream& oSt) : oStr(oSt) {}

  void operator()(const std::string& key, const std::string& value) const {
    oStr << "  - " << key << " : " << value << '\n';
  }
};

HttpVersion AHttpResponse::version() const {
  return HttpVersion(HttpVersion::HttpVersionUnknown);
}

int AHttpResponse::statusCode() const { return 0; }

const std::string& AHttpResponse::statusPhrase() const {
  static const std::string empty;
  return empty;
}

std::ostream& operator<<(std::ostream& oStr, AHttpResponse& use) {
  oStr << "Http response : " << use.version().toString() << " "
       << use.statusCode() << '\n';
  use.headers().iter(HeaderPrinter(oStr));
  return oStr;
}

}  // namespace mon_http
