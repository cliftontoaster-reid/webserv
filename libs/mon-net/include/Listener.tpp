#include <sys/socket.h>
#include <sys/types.h>

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Listener.hpp"

#ifdef OS_LINUX
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#endif

#include "Detect.hpp"
#include "HttpVersion.hpp"

namespace mon_net {

bool Context::canSniffVersion() {
  const std::string TARGET_RN = "\r\n";

  std::string::const_iterator iter = std::search(
      buffer.begin(), buffer.end(), TARGET_RN.begin(), TARGET_RN.end());

  return iter != buffer.end();
}

BufferedData::BufferedData() : file(NULL), offset(0) {}

void BufferedData::append(const char* data, size_t len) {
  buffer.insert(buffer.end(), data, data + len);
}

bool BufferedData::is_empty() const { return offset >= buffer.size(); }

void BufferedData::clear() {
  buffer.clear();
  offset = 0;
}

ssize_t BufferedData::flush(int fd) {
  if (file != NULL && buffer.size() < BUFFER_SIZE) {
    char tmp[BUFFER_SIZE] = {0};

    size_t want = BUFFER_SIZE - buffer.size();

    if (want >= BUFFER_READ_MIN) {
      size_t toRead = (want < sizeof(tmp)) ? want : sizeof(tmp);
      size_t readCount = std::fread(tmp, 1, toRead, file);

      if (readCount == 0) {
        if (std::fclose(file) != 0) {
          std::perror("fclose");
        }
        file = NULL;
      } else {
        buffer.insert(buffer.end(), tmp, tmp + readCount);
      }
    }
  }

  if (is_empty()) {
    return 0;
  }

#ifdef OS_LINUX
  ssize_t sent =
      send(fd, &buffer[offset], buffer.size() - offset, MSG_NOSIGNAL);
#else
  ssize_t sent = send(fd, &buffer[offset], buffer.size() - offset, 0);
#endif

  if (sent == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
    return -1;
  }
  if (sent > 0) {
    offset += sent;
    if (offset > (buffer.size() / 2)) {
      buffer.erase(buffer.begin(), buffer.begin() + offset);
      offset = 0;
    }
  }
  return sent;
}

template <int MaxEvents>
Listener<MaxEvents>::Listener() : epoll_fd(epoll_create(MaxEvents)) {
#ifdef OS_LINUX
  epoll_events.resize(MaxEvents);
#endif
}

template <int MaxEvents>
std::vector<Event> Listener<MaxEvents>::poll(int timeout) {
  std::vector<Event> events = wait_for_events(timeout);
  std::vector<Event> ret;
  ret.reserve(events.size());

  for (size_t i = 0; i < events.size(); ++i) {
    const Event& event = events[i];

    if (_ports.has_a(event.fd)) {
      accept_new_connection(event.fd);

      continue;
    }
    if (event.type == Event::EVENT_TYPE_OUT_OK) {
      flush_buffer(event.fd);
      continue;
    }

    ret.push_back(event);
  }

  return ret;
}

template <int MaxEvents>
void Listener<MaxEvents>::write(const std::vector<char>& data, int fd) {
  if (data.empty()) {
    return;
  }

  typename std::map<int, BufferedData>::iterator iter = _writeBuffer.find(fd);
  if (iter != _writeBuffer.end()) {
    BufferedData& entry = iter->second;
    entry.append(data.data(), data.size());
    return;
  }

#ifdef OS_LINUX
  ssize_t sent = send(fd, data.data(), data.size(), MSG_NOSIGNAL);
#else
  ssize_t sent = send(fd, data.data(), data.size(), 0);
#endif

  if (sent == -1) {
    if (errno == EPIPE || errno == ECONNRESET) {
      closeFd(fd);
      _writeBuffer.erase(fd);
      return;
    }
  }
  if (static_cast<unsigned long>(sent) != data.size()) {
    typename std::map<int, BufferedData>::iterator iter = _writeBuffer.find(fd);
    if (iter != _writeBuffer.end()) {
      BufferedData& entry = iter->second;
      entry.append(data.data() + sent, data.size() - sent);
      return;
    }

    BufferedData buffer;

    buffer.append(data.data() + sent, data.size() - sent);
    _writeBuffer.insert(std::make_pair(fd, buffer));

    ask_write(fd);
  }
}

template <int MaxEvents>
void Listener<MaxEvents>::write(mon_http::AHttpResponse& data, int fd) {
  write(data.encode(), fd);
}

template <int MaxEvents>
void Listener<MaxEvents>::write(FILE* file, int fd) {
  if (file == NULL) {
    return;
  }

  typename std::map<int, BufferedData>::iterator iter = _writeBuffer.find(fd);

  if (iter != _writeBuffer.end()) {
    if (iter->second.file != NULL) {
      std::fclose(file);  // NOLINT
      throw std::runtime_error(
          "An active file stream is already attached to this FD");
    }
    iter->second.attach_file(file);
    return;
  }

  BufferedData newBuffer;
  newBuffer.attach_file(file);

  _writeBuffer.insert(std::make_pair(fd, newBuffer));
  ask_write(fd);
}

template <int MaxEvents>
void Listener<MaxEvents>::registerPort(u_int16_t port) {
  int fd = _register_port(port);
  _ports.insert(fd, port);
}

template <int MaxEvents>
void Listener<MaxEvents>::markClose(int fd) {
  _connections[fd].closeAfterWrite = true;
}

#ifdef OS_LINUX
Event Event::fromEpoll(struct epoll_event event, int port) {
  return Event(event, port);
}

Event::Event(const struct epoll_event& EpollEvent, int port) : port(port) {
  if ((EpollEvent.events & EPOLLIN) != 0) {
    this->type = EVENT_TYPE_IN_OK;
  } else if ((EpollEvent.events & EPOLLOUT) != 0) {
    this->type = EVENT_TYPE_OUT_OK;
  } else if ((EpollEvent.events & (EPOLLHUP | EPOLLERR)) != 0) {
    this->type = EVENT_TYPE_ERROR;
  } else if ((EpollEvent.events & EPOLLRDHUP) != 0) {
    this->type = EVENT_TYPE_CLOSED;
  } else {
    throw std::runtime_error("Unknown epoll event type");
  }
  this->fd = EpollEvent.data.fd;
}

template <int MaxEvents>
void Listener<MaxEvents>::accept_new_connection(int listening_fd) {
  while (true) {
    int client_fd = accept(listening_fd, NULL, NULL);
    if (client_fd == -1) {
      break;
    }

    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

    struct epoll_event newCon;
    newCon.events = EPOLLIN | EPOLLRDHUP;
    newCon.data.fd = client_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &newCon);

    Context ctx;
    ctx.fd = client_fd;
    ctx.port = _ports.get_by_a(listening_fd);
    ctx.parser = NULL;
    ctx.version =
        mon_http::HttpVersion(mon_http::HttpVersion::HttpVersionUnknown);
    _connections[client_fd] = ctx;
  }
}

template <int MaxEvents>
std::vector<Event> Listener<MaxEvents>::wait_for_events(int timeout) {
  int nfds = epoll_wait(epoll_fd, epoll_events.data(), MaxEvents, timeout);
  if (nfds == -1) {
    if (errno == EINTR) {
      return std::vector<Event>();
    }
    throw std::runtime_error("Epoll failed");
  }

  std::vector<Event> ret;
  ret.reserve(nfds);

  for (int i = 0; i < nfds; ++i) {
    struct epoll_event& epollEvent = epoll_events[i];

    u_int16_t port = 0;
    if (_ports.has_a(epollEvent.data.fd)) {
      port = _ports.get_by_a(epollEvent.data.fd);
    } else {
      port = _connections.at(epollEvent.data.fd).port;
    }
    Event event(epollEvent, port);

    ret.push_back(event);
  }

  return ret;
}

template <int MaxEvents>
void Listener<MaxEvents>::flush_buffer(int fd) {
  typename std::map<int, BufferedData>::iterator iter = _writeBuffer.find(fd);

  if (iter == _writeBuffer.end()) {
    std::perror("fclose");
    return;
  }

  BufferedData& entry = iter->second;
  ssize_t sent = entry.flush(fd);

  if (sent == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
    _close(fd);
    close(fd);
    _connections.erase(fd);
    _writeBuffer.erase(iter);
    return;
  }
  if (sent == 0) {
    if (_connections[fd].closeAfterWrite) {
      _close(fd);
      close(fd);
      _connections.erase(fd);
      _writeBuffer.erase(iter);
    } else {
      struct epoll_event epollEvent;
      epollEvent.events = EPOLLIN;
      epollEvent.data.fd = fd;
      epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &epollEvent);
    }
    return;
  }

  if (entry.is_empty()) {
    if (entry.file == NULL && _connections[fd].closeAfterWrite) {
      _close(fd);
      close(fd);
      _connections.erase(fd);
      _writeBuffer.erase(iter);
    } else {
      struct epoll_event epollEvent;
      epollEvent.events = EPOLLIN;
      epollEvent.data.fd = fd;
      epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &epollEvent);
    }
  }
}

template <int MaxEvents>
void Listener<MaxEvents>::ask_write(int fd) {
  struct epoll_event epollEvent;
  epollEvent.events = EPOLLIN | EPOLLOUT;
  epollEvent.data.fd = fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &epollEvent);
}

template <int MaxEvents>
int Listener<MaxEvents>::_register_port(u_int16_t port) {
  int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if (fd == -1) {
    throw std::runtime_error("Could not create socket");
  }

  int optval = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
    close(fd);
    throw std::runtime_error("Could not set SO_REUSEADDR");
  }

  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1) {
    close(fd);
    throw std::runtime_error("Could not bind socket");
  }

  if (listen(fd, SOMAXCONN) == -1) {
    close(fd);
    throw std::runtime_error("Could not listen on socket");
  }

  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    close(fd);
    throw std::runtime_error("Could not add socket to epoll");
  }

  return fd;
}

#endif

}  // namespace mon_net