
#ifndef MON_HTTP_INCLUDE_FORM_HPP
#define MON_HTTP_INCLUDE_FORM_HPP

#include <sys/types.h>

#include <string>
#include <vector>

#include "HeaderMap.hpp"

namespace mon_http {

struct FormData {
  enum DataType {
    DATA_TYPE_TEXT,
    DATA_TYPE_FILE,
  };

  explicit FormData(const std::string& data);

  HeaderMap headers;
  std::string name;
  std::string filename;
  std::vector<u_int8_t> data;
  DataType type;

 private:
  void _parseContentDisposition(const std::string& value);
};

class Form {
 public:
  Form();
  explicit Form(const std::string& content_type);
  Form(const Form& other);
  Form& operator=(const Form& other);

  void parse(const std::string& body);

 private:
  std::string _delimiter;
  std::vector<FormData> _data;
};

}  // namespace mon_http

#endif