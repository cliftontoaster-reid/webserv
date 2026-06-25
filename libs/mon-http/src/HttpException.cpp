#include "HttpException.hpp"

namespace mon_http {

HttpException::HttpException(int statusCode, const char* message)
    : _statusCode(statusCode), _message(message) {}

HttpException::HttpException(const HttpException& other) throw()
    : _statusCode(other._statusCode), _message(other._message) {}

HttpException& HttpException::operator=(const HttpException& other) throw() {
  if (this != &other) {
    _statusCode = other._statusCode;
    _message = other._message;
  }
  return *this;
}

HttpException::~HttpException() throw() {}

int HttpException::statusCode() const { return _statusCode; }

const char* HttpException::what() const throw() { return _message; }

}  // namespace mon_http
