#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "Http10Request.hpp"
#include "HttpVersion.hpp"

Test(http10_request, default_construction) {
  mon_http::Http10Request req;
}

Test(http10_request, copy_construction) {
  std::string data = "GET / HTTP/1.0\r\n\r\n";
  std::vector<char> raw(data.begin(), data.end());
  mon_http::Http10Request req;
  req = req.parse(raw);

  mon_http::Http10Request copy(req);
  cr_assert_eq(copy.method().getType(), mon_http::HttpMethod::HttpMethodGet);
}

Test(http10_request, assignment_operator) {
  std::string data = "POST /resource HTTP/1.0\r\n\r\n";
  std::vector<char> raw(data.begin(), data.end());
  mon_http::Http10Request req;
  req = req.parse(raw);

  mon_http::Http10Request assigned;
  assigned = req;
  cr_assert_eq(assigned.method().getType(),
               mon_http::HttpMethod::HttpMethodPost);
}

Test(http10_request, version) {
  mon_http::Http10Request req;
  cr_assert_eq(req.version().value, mon_http::HttpVersion::HttpVersion1_0);
}

Test(http10_request, parse_simple_get) {
  std::string data = "GET / HTTP/1.0\r\n\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  req = req.parse(raw);

  cr_assert_eq(req.method().getType(), mon_http::HttpMethod::HttpMethodGet);
  cr_assert_eq(req.path(), "/");
}

Test(http10_request, parse_with_path) {
  std::string data = "GET /index.html HTTP/1.0\r\n\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  req = req.parse(raw);

  cr_assert_eq(req.path(), "/index.html");
}

Test(http10_request, parse_with_headers) {
  std::string data =
      "GET / HTTP/1.0\r\n"
      "Host: example.com\r\n"
      "User-Agent: curl/8.0\r\n"
      "\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  req = req.parse(raw);

  cr_assert_eq(req.method().getType(), mon_http::HttpMethod::HttpMethodGet);
  cr_assert_eq(req.path(), "/");
}

Test(http10_request, parse_with_body) {
  std::string data =
      "POST /submit HTTP/1.0\r\n"
      "Content-Length: 12\r\n"
      "\r\n"
      "Hello World!";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  req = req.parse(raw);

  cr_assert_eq(req.method().getType(), mon_http::HttpMethod::HttpMethodPost);
  cr_assert_eq(req.path(), "/submit");
}

Test(http10_request, parse_wrong_version_throws) {
  std::string data = "GET / HTTP/1.1\r\n\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  cr_assert_throw(req.parse(raw), std::runtime_error);
}

Test(http10_request, parse_missing_line_terminator_throws) {
  std::string data = "GET / HTTP/1.0";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  cr_assert_throw(req.parse(raw), std::runtime_error);
}

Test(http10_request, parse_missing_space_after_method_throws) {
  std::string data = "GET\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  cr_assert_throw(req.parse(raw), std::runtime_error);
}

Test(http10_request, parse_missing_space_after_path_throws) {
  std::string data = "GET /index.html\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  cr_assert_throw(req.parse(raw), std::runtime_error);
}

Test(http10_request, method_accessor) {
  std::string data = "DELETE /resource HTTP/1.0\r\n\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  req = req.parse(raw);

  cr_assert_eq(req.method().getType(),
               mon_http::HttpMethod::HttpMethodDelete);
}

Test(http10_request, has_header_after_parse) {
  std::string data =
      "GET / HTTP/1.0\r\n"
      "Host: example.com\r\n"
      "\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  req = req.parse(raw);

  cr_assert_eq(req.hasHeader("Host"), true);
  cr_assert_eq(req.header("Host"), "example.com");
}

Test(http10_request, has_header_case_insensitive) {
  std::string data =
      "GET / HTTP/1.0\r\n"
      "X-Custom-Header: my-value\r\n"
      "\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  req = req.parse(raw);

  cr_assert_eq(req.hasHeader("x-custom-header"), true);
  cr_assert_eq(req.header("X-Custom-Header"), "my-value");
}

Test(http10_request, has_header_missing_returns_false) {
  mon_http::Http10Request req;
  cr_assert_eq(req.hasHeader("Nonexistent"), false);
}

Test(http10_request, header_throws_if_missing) {
  mon_http::Http10Request req;
  cr_assert_throw(req.header("Nonexistent"), std::out_of_range);
}

Test(http10_request, media_type_throws_when_missing) {
  mon_http::Http10Request req;
  cr_assert_throw(req.mediaType(), std::runtime_error);
}

Test(http10_request, headers_returns_reference) {
  std::string data =
      "GET / HTTP/1.0\r\n"
      "Host: example.com\r\n"
      "\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  req = req.parse(raw);

  mon_http::HeaderMap& h = req.headers();
  cr_assert_eq(h.at("Host"), "example.com");
}

Test(http10_request, body_content_length_mismatch_throws) {
  std::string data =
      "POST /submit HTTP/1.0\r\n"
      "Content-Length: 99\r\n"
      "\r\n"
      "Actual body is shorter";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10Request req;
  cr_assert_throw(req.parse(raw), std::runtime_error);
}
