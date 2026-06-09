#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "AHttpRequest.hpp"
#include "AHttpStreamParser.hpp"
#include "Http10Request.hpp"
#include "Http10StreamParser.hpp"

Test(http10_stream_parser, default_construction) {
  mon_http::Http10StreamParser parser;
}

Test(http10_stream_parser, copy_construction) {
  mon_http::Http10StreamParser parser;
  mon_http::Http10StreamParser copy(parser);
}

Test(http10_stream_parser, assignment_operator) {
  mon_http::Http10StreamParser parser;
  mon_http::Http10StreamParser copy;
  copy = parser;
}

Test(http10_stream_parser, empty_buffer_cannot_pull) {
  mon_http::Http10StreamParser parser;
  cr_assert_eq(parser.canPull(), false);
}

Test(http10_stream_parser, partial_request_cannot_pull) {
  std::string data = "GET / HTTP/1.0\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10StreamParser parser;
  parser.append(raw);

  cr_assert_eq(parser.canPull(), false);
}

Test(http10_stream_parser, full_request_can_pull) {
  std::string data = "GET / HTTP/1.0\r\n\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10StreamParser parser;
  parser.append(raw);

  cr_assert_eq(parser.canPull(), true);
}

Test(http10_stream_parser, request_with_body_can_pull) {
  std::string data =
      "POST /submit HTTP/1.0\r\n"
      "Content-Length: 12\r\n"
      "\r\n"
      "Hello World!";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10StreamParser parser;
  parser.append(raw);

  cr_assert_eq(parser.canPull(), true);
}

Test(http10_stream_parser, pull_returns_null_when_empty) {
  mon_http::Http10StreamParser parser;
  cr_assert_eq(parser.pull(), NULL);
}

Test(http10_stream_parser, pull_returns_null_on_partial) {
  std::string data = "GET / HTTP/1.0\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10StreamParser parser;
  parser.append(raw);

  cr_assert_eq(parser.pull(), NULL);
}

Test(http10_stream_parser, pull_returns_valid_request) {
  std::string data = "GET / HTTP/1.0\r\n\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10StreamParser parser;
  parser.append(raw);

  mon_http::AHttpRequest* req = parser.pull();
  cr_assert_not_null(req);
  cr_assert_eq(req->method().getType(), mon_http::HttpMethod::HttpMethodGet);
  cr_assert_eq(req->path(), "/");
  delete req;
}

Test(http10_stream_parser, pull_request_with_headers) {
  std::string data =
      "GET /index.html HTTP/1.0\r\n"
      "Host: example.com\r\n"
      "User-Agent: curl/8.0\r\n"
      "\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10StreamParser parser;
  parser.append(raw);

  mon_http::AHttpRequest* req = parser.pull();
  cr_assert_not_null(req);
  cr_assert_eq(req->hasHeader("Host"), true);
  cr_assert_eq(req->header("Host"), "example.com");
  cr_assert_eq(req->hasHeader("User-Agent"), true);
  cr_assert_eq(req->header("User-Agent"), "curl/8.0");
  delete req;
}

Test(http10_stream_parser, pull_request_with_body) {
  std::string data =
      "POST /submit HTTP/1.0\r\n"
      "Content-Length: 12\r\n"
      "\r\n"
      "Hello World!";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10StreamParser parser;
  parser.append(raw);

  mon_http::AHttpRequest* req = parser.pull();
  cr_assert_not_null(req);
  cr_assert_eq(req->method().getType(), mon_http::HttpMethod::HttpMethodPost);
  cr_assert_eq(req->path(), "/submit");
  cr_assert_eq(req->hasBody(), true);
  cr_assert_eq(req->body(), "Hello World!");
  delete req;
}

Test(http10_stream_parser, pull_returns_null_on_body_mismatch) {
  std::string data =
      "POST /submit HTTP/1.0\r\n"
      "Content-Length: 99\r\n"
      "\r\n"
      "Actual body is shorter";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10StreamParser parser;
  parser.append(raw);

  cr_assert_eq(parser.pull(), NULL);
}

Test(http10_stream_parser, pull_throws_on_wrong_version) {
  std::string data = "GET / HTTP/1.1\r\n\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10StreamParser parser;
  parser.append(raw);

  cr_assert_throw(parser.pull(), std::runtime_error);
}

Test(http10_stream_parser, pull_throws_on_malformed_request) {
  std::string data = "GET\r\n\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10StreamParser parser;
  parser.append(raw);

  cr_assert_throw(parser.pull(), std::runtime_error);
}

Test(http10_stream_parser, append_in_chunks) {
  mon_http::Http10StreamParser parser;

  std::string chunk1 = "GET / HTTP/1.0\r\n";
  std::vector<char> raw1(chunk1.begin(), chunk1.end());
  parser.append(raw1);

  cr_assert_eq(parser.canPull(), false);

  std::string chunk2 = "\r\n";
  std::vector<char> raw2(chunk2.begin(), chunk2.end());
  parser.append(raw2);

  cr_assert_eq(parser.canPull(), true);
}

Test(http10_stream_parser, pull_does_not_clear_buffer) {
  std::string data = "GET / HTTP/1.0\r\n\r\n";
  std::vector<char> raw(data.begin(), data.end());

  mon_http::Http10StreamParser parser;
  parser.append(raw);

  mon_http::AHttpRequest* req = parser.pull();
  cr_assert_not_null(req);
  delete req;

  cr_assert_eq(parser.canPull(), true);
}

Test(http10_stream_parser, incremental_append_headers_then_body) {
  mon_http::Http10StreamParser parser;

  std::string chunk1 =
      "POST /submit HTTP/1.0\r\n"
      "Content-Length: 12\r\n"
      "\r\n";
  std::vector<char> raw1(chunk1.begin(), chunk1.end());
  parser.append(raw1);

  cr_assert_eq(parser.canPull(), true);

  mon_http::AHttpRequest* req1 = parser.pull();
  cr_assert_null(req1);
}
