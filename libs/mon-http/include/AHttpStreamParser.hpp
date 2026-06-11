
#ifndef MON_HTTP_INCLUDE_AHTTPSTREAMPARSER_HTTP
#define MON_HTTP_INCLUDE_AHTTPSTREAMPARSER_HTTP

#include <string>
#include <vector>

#include "AHttpRequest.hpp"
namespace mon_http {

class AHttpStreamParser {
 public:
  AHttpStreamParser();
  AHttpStreamParser(const AHttpStreamParser& other);
  AHttpStreamParser& operator=(const AHttpStreamParser& other);
  virtual ~AHttpStreamParser();

  virtual void append(const std::vector<char>& source) {
    _buffer.reserve(_buffer.size() + source.size());
    _buffer.insert(_buffer.end(), source.begin(), source.end());
  }
  virtual void append(const std::string& source) {
    _buffer.reserve(_buffer.size() + source.length());
    _buffer.insert(_buffer.end(), source.begin(), source.end());
  }

  virtual AHttpRequest* pull();
  virtual bool canPull();

 protected:
  std::vector<char> _buffer;
};

}  // namespace mon_http

#endif
