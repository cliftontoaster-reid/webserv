#ifndef MON_NET_INCLUDE_LISTENER_HTTP
#define MON_NET_INCLUDE_LISTENER_HTTP

#include <sys/types.h>
#include <unistd.h>

#include <cstddef>
#include <cstdio>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "AHttpResponse.hpp"
#include "AHttpStreamParser.hpp"
#include "Detect.hpp"
#include "DualMap.hpp"
#include "HttpVersion.hpp"

#ifdef OS_LINUX
#include <sys/epoll.h>
#endif

namespace mon_net {

struct Event {
  // TODO(cliftontoaster-reid) Use a bitmap
  enum Type {
    EVENT_TYPE_IN_OK,
    EVENT_TYPE_OUT_OK,
    EVENT_TYPE_CLOSED,
    EVENT_TYPE_ERROR,
  };

  Type type;
  u_int16_t port;
  int fd;

#ifdef OS_LINUX
  static Event fromEpoll(struct epoll_event event, int port);
  explicit Event(const struct epoll_event& EpollEvent, int port);
#endif
};

struct Context {
  int fd;
  int port;
  bool closeAfterWrite;
  mon_http::HttpVersion version;
  mon_http::AHttpStreamParser* parser;
  std::string buffer;

  Context() : fd(-1), port(0), closeAfterWrite(false), parser(NULL) {}

  bool canSniffVersion();
};

struct BufferedData {
  std::vector<char> buffer;
  FILE* file;
  size_t offset;

  BufferedData();
  void append(const char* data, size_t len);
  bool is_empty() const;
  void clear();
  ssize_t flush(int fd);
  void attach_file(FILE* filePtr) {
    if (file != NULL) {
      std::fclose(file);  // NOLINT
    }
    file = filePtr;
  }
};

template <int MaxEvents>
class Listener {
 public:
  Listener();

  std::vector<Event> poll(int timeout);
  void write(const std::vector<char>& data, int fd);
  void write(mon_http::AHttpResponse& data, int fd);
  void write(FILE* file, int fd);

  void registerPort(u_int16_t port);
  void markClose(int fd);
  void closeFd(int fd) {
    std::map<int, Context>::iterator iter = _connections.find(fd);
    if (iter == _connections.end()) {
      throw std::runtime_error("Cannot find connection to close");
    }

    _close(fd);
    close(fd);
    _connections.erase(fd);
  }

  Context& connContext(int fd) {
    std::map<int, Context>::iterator iter = _connections.find(fd);
    if (iter == _connections.end()) {
      throw std::runtime_error("Cannot find connection");
    }
    return iter->second;
  }

 private:
  DualMap<int, u_int16_t> _ports;
  std::map<int, Context> _connections;
  std::map<int, BufferedData> _writeBuffer;

#ifdef OS_LINUX
  int epoll_fd;
  std::vector<struct epoll_event> epoll_events;

  void accept_new_connection(int listening_fd);
  std::vector<Event> wait_for_events(int timeout);
  void flush_buffer(int fd);
  void ask_write(int fd);

  int _register_port(u_int16_t);
  void _close(int fd) { epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL); }
#endif
};

}  // namespace mon_net

#include "Listener.tpp"

#endif
