#ifndef WEBSERV_INCLUDE_FUNCS_HPP
#define WEBSERV_INCLUDE_FUNCS_HPP

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "AHttpRequest.hpp"
#include "AHttpResponse.hpp"
#include "Form.hpp"
#include "Handler.hpp"
#include "HttpException.hpp"
#include "Path.hpp"

#define FILENAME_RANDOM_LEN 32

std::string random(size_t len) {
  static unsigned char buf[2048];
  static size_t pos = sizeof(buf);
  std::string result;
  result.reserve(len * 2);

  for (size_t i = 0; i < len; ++i) {
    if (pos >= sizeof(buf)) {
      std::ifstream file("/dev/urandom", std::ios::binary);
      file.read(reinterpret_cast<char*>(buf), sizeof(buf));
      pos = 0;
    }

    unsigned char byte = buf[pos++];
    char hex[3];
    sprintf(hex, "%02x", byte);
    result += hex;
  }

  return result;
}

std::string scramble_name(const std::string& name, size_t len) {
  size_t dot = name.find('.');
  if (dot == std::string::npos) {
    return name + "_" + random(len);
  }

  return name.substr(0, dot) + "_" + random(len) +
         name.substr(dot, name.size());
}

void randomFile(const std::string& root, const std::string& filename,
                std::ofstream& file_write, size_t len) {
  for (int i = 0; i < 1000; ++i) {
    mon_router::Path p(root);
    p.append(scramble_name(filename, len));

    std::string file_path;
    if (p.resolve(file_path)) {
      continue;
    }

    std::ifstream file_read;
    file_read.open(file_path.c_str());
    if (file_read.is_open()) {
      file_read.close();
      continue;
    }

    file_write.open(file_path.c_str());
    if (!file_write.is_open()) {
      continue;
    }

    std::ofstream file_name_write;
    file_name_write.open((file_path + "._txt").c_str());
    if (!file_write.is_open()) {
      file_write.close();
      continue;
    }

    file_name_write << filename << std::endl;
    file_name_write.close();
    return;
  }

  throw mon_http::HttpException(STATUS_Internal_Server_Error,
                                "Could not write '" + filename + "' to disk.");
}

namespace webserv {

// HandlerResponse (*func)(mon_http::AHttpRequest &, mon_http::Form &, )

inline mon_router::HandlerResponse echo(
    mon_http::AHttpRequest& request, mon_http::Form& form,
    const std::map<std::string, toml98::Value>& arguments) {
  mon_router::HandlerResponse res;
  (void)form;
  (void)arguments;

  if (request.hasBody()) {
    res.body = request.body();
  }
  res.code = STATUS_OK;
  res.message = "OK";
  res.headers.extend(request.headers());

  return res;
}

inline mon_router::HandlerResponse upload(
    mon_http::AHttpRequest& request, mon_http::Form& form,
    const std::map<std::string, toml98::Value>& arguments) {
  std::vector<mon_http::FormData>::const_iterator iter;

  for (iter = form.begin(); iter != form.end(); iter++) {
    const mon_http::FormData& data = *iter;
    if (data.type == mon_http::FormData::DATA_TYPE_FILE) {
      std::ofstream file;
      randomFile(*arguments.at("path").getString(), data.filename, file,
                 arguments.at("random_len").getInteger());
      file.write(reinterpret_cast<const char*>(&data.data[0]),
                 data.data.size());
      file.flush();
      file.close();
    }
  }

  (void)request;
  mon_router::HandlerResponse res;
  res.code = STATUS_No_Content;
  res.message = "No Content";
  return res;
}

}  // namespace webserv

#endif