
#ifndef MON_HTTP_INCLUDE_HTTP10STREAMPARSER_HTTP
#define MON_HTTP_INCLUDE_HTTP10STREAMPARSER_HTTP

#include <string>

#include "AHttpStreamParser.hpp"

namespace mon_http {

class Http10StreamParser : public AHttpStreamParser {
 public:
  Http10StreamParser();
  explicit Http10StreamParser(const std::string& str);
  Http10StreamParser(const Http10StreamParser& other);
  Http10StreamParser& operator=(const Http10StreamParser& other);
  virtual ~Http10StreamParser();

  AHttpRequest* pull();
  bool canPull();
};

}  // namespace mon_http

#endif
