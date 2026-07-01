#ifndef MON_HTTP_INCLUDE_HTTPEXCEPTION_HPP
#define MON_HTTP_INCLUDE_HTTPEXCEPTION_HPP

#include <exception>

namespace mon_http {

class HttpException : public std::exception {
 public:
  HttpException(int statusCode, const char* message);
  HttpException(const HttpException& other) throw();
  HttpException& operator=(const HttpException& other) throw();
  virtual ~HttpException() throw();

  int statusCode() const;
  virtual const char* what() const throw();

 private:
  int _statusCode;
  const char* _message;
};

}  // namespace mon_http

#endif
