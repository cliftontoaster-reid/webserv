#include "Server.hpp"

#include <sys/types.h>
#include <unistd.h>

#include <exception>

#include "AHttpResponse.hpp"
#include "Http10Response.hpp"
#include "Http10StreamParser.hpp"
#include "HttpVersion.hpp"
#include "Listener.hpp"

namespace mon_net {

Server::Server() {}
Server::Server(const Server& other) : _listener(other._listener) {}
Server& Server::operator=(const Server& other) {
  if (this != &other) {
    _listener = other._listener;
  }
  return *this;
}

void Server::registerPort(u_int16_t port) { _listener.registerPort(port); }

void Server::run() {
  while (true) {
    std::vector<Event> events = _listener.poll(-1);

    for (size_t i = 0; i < events.size(); ++i) {
      const Event& event = events[i];

      if (event.type == Event::EVENT_TYPE_IN_OK) {
        try {
          Context& ctx = _listener.connContext(event.fd);
          if (readVersion(ctx)) {
            switch (ctx.version.value) {
              case mon_http::HttpVersion::HttpVersion1_0:
              case mon_http::HttpVersion::HttpVersion1_1: {
                ctx.parser = new mon_http::Http10StreamParser();
                ctx.parser->append(ctx.buffer);
                ctx.buffer.clear();
                try {
                  if (ctx.parser->canPull()) {
                    mon_http::AHttpRequest* req = ctx.parser->pull();
                    _router.handle(*req, ctx.fd, _listener);
                    delete req;
                  } else {
                    throw std::runtime_error("Bad Request");
                  }
                } catch (std::exception& err) {
                  mon_http::Http10Response res;
                  res.statusMessage = err.what();
                  res.setStatusCode(STATUS_Bad_Request);
                  _listener.markClose(ctx.fd);
                  _listener.write(res, ctx.fd);
                }
                break;
              }

              case mon_http::HttpVersion::HttpVersionUnknown:
              case mon_http::HttpVersion::HttpVersion0_9:
              case mon_http::HttpVersion::HttpVersion2_0:
                mon_http::Http10Response res = mon_http::Http10Response();
                res.setStatusCode(STATUS_Version_Not_Supported);
                res.statusMessage = "HTTP Version Not Supported";
                _listener.markClose(ctx.fd);
                _listener.write(res, event.fd);
                close(ctx.fd);
                break;
            }
          }
        } catch (std::exception& e) {
          mon_http::Http10Response res = mon_http::Http10Response();
          res.setStatusCode(STATUS_Internal_Server_Error);
          res.statusMessage = "Internal Server Error";
        }
      }
    }
  }
}

bool Server::readVersion(Context& ctx) {
  readCtx(ctx);
  if (ctx.version.value == mon_http::HttpVersion::HttpVersionUnknown &&
      ctx.canSniffVersion()) {
    mon_http::HttpVersion ver =
        mon_http::HttpVersion::sniffHttpVersion(ctx.buffer);
    ctx.version = ver;

    switch (ver.value) {
      case mon_http::HttpVersion::HttpVersionUnknown:
        return false;
      case mon_http::HttpVersion::HttpVersion1_0:
      case mon_http::HttpVersion::HttpVersion1_1:
      case mon_http::HttpVersion::HttpVersion0_9:
      case mon_http::HttpVersion::HttpVersion2_0:
        return true;
    }
  }
  return ctx.version.value != mon_http::HttpVersion::HttpVersionUnknown;
}

void Server::readCtx(Context& ctx) {
  char tmp[1024];

  while (true) {
    ssize_t readCount = read(ctx.fd, tmp, sizeof(tmp));
    if (readCount < 1) {
      break;
    }

    ctx.buffer.insert(ctx.buffer.end(), tmp, tmp + readCount);
  }
}

}  // namespace mon_net