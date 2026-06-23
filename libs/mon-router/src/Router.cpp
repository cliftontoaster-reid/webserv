#include "Router.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>

#include "Detect.hpp"
#include "Http10Response.hpp"
#include "Listener.hpp"
#include "Path.hpp"
#include "Uri.hpp"

namespace {

std::string random(size_t len) {
  static unsigned char buf[2048];
  static size_t pos = sizeof(buf);

  const std::string charset =
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789";

  std::string result;
  result.reserve(len);

  while (result.length() < len) {
    if (pos >= sizeof(buf)) {
      std::ifstream file("/dev/urandom", std::ios::binary);
      if (!file.is_open()) {
        throw std::runtime_error("Failed to open /dev/urandom");
      }
      file.read(reinterpret_cast<char*>(buf), sizeof(buf));
      pos = 0;
    }

    unsigned char randomByte = buf[pos++];
    result += charset[randomByte % charset.length()];
  }

  return result;
}

inline std::string getNewFileName(const std::string& file_name) {
  size_t last_slash = file_name.find_last_of(PATH_SEPARATOR);

  if (last_slash == std::string::npos) {
    return random(16) + "_" + file_name;
  }

  return file_name.substr(0, last_slash + sizeof(PATH_SEPARATOR)) + random(16) +
         "_" + file_name.substr(last_slash + sizeof(PATH_SEPARATOR));
}

inline std::string get_mime_type(const std::string& path) {
  static std::map<std::string, std::string> mime_types;
  if (mime_types.empty()) {
    mime_types[".html"] = "text/html";
    mime_types[".htm"] = "text/html";
    mime_types[".css"] = "text/css";
    mime_types[".js"] = "application/javascript";
    mime_types[".png"] = "image/png";
    mime_types[".jpg"] = "image/jpeg";
    mime_types[".jpeg"] = "image/jpeg";
    mime_types[".gif"] = "image/gif";
    mime_types[".ico"] = "image/x-icon";
    mime_types[".json"] = "application/json";
    mime_types[".txt"] = "text/plain";
  }

  size_t dot_idx = path.find_last_of('.');
  if (dot_idx == std::string::npos) {
    return "application/octet-stream";
  }

  std::string ext = path.substr(dot_idx);
  std::map<std::string, std::string>::const_iterator iter =
      mime_types.find(ext);
  if (iter != mime_types.end()) {
    return iter->second;
  }

  return "application/octet-stream";
}

}  // namespace

namespace mon_router {

Router::Router() {}
Router::Router(const Router& other) { (void)other; }
Router& Router::operator=(const Router& other) {
  if (this != &other) {
  }
  return *this;
}
Router::~Router() {}

void Router::addRoute(const std::string& prefix, const std::string& path,
                      u_int16_t port) {
  Route route;
  route.preffix = prefix;
  route.path = path;
  route.port = port;
  _paths.push_back(route);
}

template <int MaxEvents>
void Router::handle(mon_http::AHttpRequest& request, int client_fd,
                    mon_net::Listener<MaxEvents>& listener) {
  try {
    const std::string& pathRaw = request.path();
    Uri uri(pathRaw);
    const Route* route = find_match(uri.path());
    if (!route) {
      mon_http::Http10Response res;
      res.error404();

      listener.markClose(client_fd);
      listener.write(res, client_fd);
      return;
    }
    Path path(route->path);
    path.append(uri.path().substr(route->preffix.length()));
    std::string full_path;
    if (!path.resolve(full_path)) {
      mon_http::Http10Response res;
      res.error404();

      listener.markClose(client_fd);
      listener.write(res, client_fd);
      return;
    }

    mon_http::Http10Response res;
    res.ok200();
    res.headers().insert("Content-Type", get_mime_type(full_path));

    FILE* file = std::fopen(full_path.c_str(), "rb");
    if (file == NULL) {
      throw std::runtime_error("Could not read file");
    }
    std::fseek(file, 0, SEEK_END);  // NOLINT(cert-err33-c)
    long fsize = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);  // NOLINT(cert-err33-c)
    std::string body(fsize, '\0');
    if (fsize > 0 && std::fread(&body[0], 1, fsize, file) == 0) {
      std::fclose(file);  // NOLINT(cert-err33-c)
      throw std::runtime_error("Could not read file content");
    }
    std::fclose(file);  // NOLINT(cert-err33-c)
    res.setBody(body);

    listener.write(res, client_fd);
    listener.markClose(client_fd);

  } catch (std::exception& err) {
    mon_http::Http10Response res;
    res.statusMessage = err.what();
    res.error500(err.what());

    listener.markClose(client_fd);
    listener.write(res, client_fd);
  }
}

template void Router::handle<MAX_EVENTS>(
    mon_http::AHttpRequest& request, int client_fd,
    mon_net::Listener<MAX_EVENTS>& listener);

}  // namespace mon_router