#include "AHttpStreamParser.hpp"

namespace mon_http {

AHttpStreamParser::AHttpStreamParser() {}
AHttpStreamParser::AHttpStreamParser(const AHttpStreamParser& other)
    : _buffer(other._buffer) {}
AHttpStreamParser& AHttpStreamParser::operator=(
    const AHttpStreamParser& other) {
  if (this != &other) {
    _buffer = other._buffer;
  }
  return *this;
}
AHttpStreamParser::~AHttpStreamParser() {}

AHttpRequest* AHttpStreamParser::pull() { return NULL; }
bool AHttpStreamParser::canPull() { return false; }

}  // namespace mon_http
