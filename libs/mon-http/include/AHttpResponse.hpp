
#ifndef MON_HTTP_INCLUDE_AHTTPRESPONSE_HPP
#define MON_HTTP_INCLUDE_AHTTPRESPONSE_HPP

#include <sys/types.h>

#include <ostream>
#include <string>
#include <vector>

#include "HeaderMap.hpp"
#include "HttpVersion.hpp"

#define STATUS_OK 200
#define STATUS_Created 201
#define STATUS_Accepted 202
#define STATUS_No_Content 204
#define STATUS_Moved_Permanently 301
#define STATUS_Found 302
#define STATUS_Not_Modified 304
#define STATUS_Temporary_redirect 307
#define STATUS_Bad_Request 400
#define STATUS_Unauthorized 401
#define STATUS_Forbidden 403
#define STATUS_Not_Found 404
#define STATUS_Request_Entity_Too_Large 413
#define STATUS_Request_URI_Too_Long 414
#define STATUS_Internal_Server_Error 500
#define STATUS_Not_Implemented 501
#define STATUS_Bad_Gateway 502
#define STATUS_Service_Unavailable 503
#define STATUS_Version_Not_Supported 505

namespace mon_http {

class AHttpResponse {
 public:
  virtual ~AHttpResponse() {}

  virtual std::vector<char> encode() = 0;

  virtual HttpVersion version() const;
  virtual int statusCode() const;
  virtual void setStatusCode(int code) = 0;
  virtual void setStatusPhrase(const std::string& message) = 0;
  virtual const std::string& statusPhrase() const;
  virtual bool hasBody() const = 0;
  std::string& body();
  virtual void setBody(const std::string& body) = 0;
  virtual void appendBody(const std::string& str) = 0;
  virtual const std::string& body() const = 0;
  virtual bool hasHeader(const std::string& key) = 0;
  virtual const std::string& header(const std::string& key) = 0;
  virtual HeaderMap& headers() = 0;

  virtual void error500(const std::string& message) = 0;
  virtual void error404() = 0;
  virtual void ok200() = 0;
};

std::ostream& operator<<(std::ostream& oStr, AHttpResponse& use);

}  // namespace mon_http

#endif