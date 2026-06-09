#include "Http10StreamParser.hpp"

#include "Http10Request.hpp"

namespace mon_http {

Http10StreamParser::Http10StreamParser() {}
Http10StreamParser::Http10StreamParser(const Http10StreamParser& other)
    : AHttpStreamParser(other) {
  (void)other;
}
Http10StreamParser& Http10StreamParser::operator=(
    const Http10StreamParser& other) {
  (void)other;
  if (this != &other) {
  }
  return *this;
}
Http10StreamParser::~Http10StreamParser() {}

AHttpRequest* Http10StreamParser::pull() {
  if (!canPull()) {
    return NULL;
  }

  Http10Request* ret = new Http10Request();

  try {
    ret->parse(_buffer);
    return ret;
  } catch (EndedTooEarly& err) {
    delete ret;
    return NULL;
  } catch (std::exception& err) {
    delete ret;
    throw err;
  }
}
bool Http10StreamParser::canPull() {
  return Http10Request::isFullRequest(_buffer);
}

}  // namespace mon_http