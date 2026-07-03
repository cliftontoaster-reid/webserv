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
  std::cerr << "DEBUG random: len=" << len << std::endl;
  static unsigned char buf[2048];
  static size_t pos = sizeof(buf);
  std::string result;
  result.reserve(len * 2);

  for (size_t i = 0; i < len; ++i) {
    if (pos >= sizeof(buf)) {
      std::cerr << "DEBUG random: reading /dev/urandom" << std::endl;
      std::ifstream file("/dev/urandom", std::ios::binary);
      file.read(reinterpret_cast<char*>(buf), sizeof(buf));
      pos = 0;
    }

    unsigned char byte = buf[pos++];
    char hex[3];
    sprintf(hex, "%02x", byte);
    result += hex;
  }

  std::cerr << "DEBUG random: done, result.size=" << result.size() << std::endl;
  return result;
}

std::string scramble_name(const std::string& name, size_t len) {
  size_t dot = name.find('.');
  if (dot == std::string::npos) {
    return name + "_" + random(len);
  }

  return name.substr(0, dot) + random(len) + "_" +
         name.substr(dot + 1, name.size());
}

void randomFile(const std::string& root, const std::string& filename,
                std::ofstream& file_write, size_t len) {
  std::cerr << "DEBUG randomFile: root=" << root << " filename=" << filename
            << " len=" << len << std::endl;
  for (int i = 0; i < 1000; ++i) {
    std::cerr << "DEBUG randomFile: attempt " << i << std::endl;
    mon_router::Path p(root);
    p.append(scramble_name(filename, len));

    std::string file_path;
    if (p.resolve(file_path)) {
      std::cerr << "DEBUG randomFile: resolve returned true, path=" << file_path << std::endl;
      continue;
    }

    std::ifstream file_read;
    file_read.open(file_path.c_str());
    if (file_read.is_open()) {
      std::cerr << "DEBUG randomFile: file exists, path=" << file_path << std::endl;
      file_read.close();
      continue;
    }

    file_write.open(file_path.c_str());
    if (!file_write.is_open()) {
      std::cerr << "DEBUG randomFile: failed to open for writing, path=" << file_path << std::endl;
      continue;
    }
    std::cerr << "DEBUG randomFile: success, path=" << file_path << std::endl;
    return;
  }

  std::cerr << "DEBUG randomFile: all 1000 attempts failed, throwing" << std::endl;
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
  std::cerr << "DEBUG upload: entry, form size=" << form.end() - form.begin() << std::endl;
  std::vector<mon_http::FormData>::const_iterator iter;

  for (iter = form.begin(); iter != form.end(); iter++) {
    const mon_http::FormData& data = *iter;
    std::cerr << "DEBUG upload: part type=" << data.type
              << " name=" << data.name
              << " filename=" << data.filename
              << " data.size=" << data.data.size() << std::endl;
    if (data.type == mon_http::FormData::DATA_TYPE_FILE) {
      std::ofstream file;
      std::cerr << "DEBUG upload: calling randomFile" << std::endl;
      randomFile(*arguments.at("path").getString(), data.filename, file,
                 arguments.at("random_len").getInteger());
      std::cerr << "DEBUG upload: randomFile returned, writing " << data.data.size() << " bytes" << std::endl;
      file.write(reinterpret_cast<const char*>(&data.data[0]),
                 data.data.size());
      std::cerr << "DEBUG upload: write done, flushing" << std::endl;
      file.flush();
      std::cerr << "DEBUG upload: flush done, closing" << std::endl;
      file.close();
      std::cerr << "DEBUG upload: close done" << std::endl;
    }
  }

  (void)request;
  std::cerr << "DEBUG upload: building response" << std::endl;
  mon_router::HandlerResponse res;
  res.code = STATUS_No_Content;
  res.message = "No Content";
  std::cerr << "DEBUG upload: returning" << std::endl;
  return res;
}

}  // namespace webserv

#endif