#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "AHttpResponse.hpp"
#include "Http10Response.hpp"

Test(http10_response, default_construction) { mon_http::Http10Response res; }

Test(http10_response, copy_construction) {
  mon_http::Http10Response res;
  res.setStatusCode(STATUS_OK);
  res.statusMessage = "OK";
  res.body() = "hello";
  res.headers().insert("Host", "example.com");

  mon_http::Http10Response copy(res);
  cr_assert_eq(copy.statusCode(), 200);
  cr_assert_eq(copy.statusMessage, "OK");
  cr_assert_eq(copy.body(), "hello");
  cr_assert_eq(copy.header("Host"), "example.com");
}

Test(http10_response, assignment_operator) {
  mon_http::Http10Response res;
  res.setStatusCode(STATUS_Not_Found);
  res.statusMessage = "Not Found";
  res.body() = "error";
  res.headers().insert("Content-Type", "text/plain");

  mon_http::Http10Response copy;
  copy = res;
  cr_assert_eq(copy.statusCode(), 404);
  cr_assert_eq(copy.statusMessage, "Not Found");
  cr_assert_eq(copy.body(), "error");
  cr_assert_eq(copy.header("Content-Type"), "text/plain");
}

Test(http10_response, version) {
  mon_http::Http10Response res;
  cr_assert_eq(res.version().value, mon_http::HttpVersion::HttpVersion1_0);
}

Test(http10_response, status_code_default) {
  mon_http::Http10Response res;
  cr_assert_eq(res.statusCode(), 0);
}

Test(http10_response, set_status_code) {
  mon_http::Http10Response res;
  res.setStatusCode(STATUS_OK);
  cr_assert_eq(res.statusCode(), 200);
}

Test(http10_response, set_status_code_invalid_negative) {
  mon_http::Http10Response res;
  cr_assert_throw(res.setStatusCode(-1), std::runtime_error);
}

Test(http10_response, set_status_code_invalid_over_999) {
  mon_http::Http10Response res;
  cr_assert_throw(res.setStatusCode(1000), std::runtime_error);
}

Test(http10_response, has_body_empty) {
  mon_http::Http10Response res;
  cr_assert_eq(res.hasBody(), false);
}

Test(http10_response, has_body_non_empty) {
  mon_http::Http10Response res;
  res.body() = "hello";
  cr_assert_eq(res.hasBody(), true);
}

Test(http10_response, has_header_without_headers) {
  mon_http::Http10Response res;
  cr_assert_eq(res.hasHeader("Host"), false);
}

Test(http10_response, has_header_with_header) {
  mon_http::Http10Response res;
  res.headers().insert("Host", "example.com");
  cr_assert_eq(res.hasHeader("Host"), true);
}

Test(http10_response, header_throws_if_missing) {
  mon_http::Http10Response res;
  cr_assert_throw(res.header("Nonexistent"), std::out_of_range);
}

Test(http10_response, encode_basic) {
  mon_http::Http10Response res;
  res.setStatusCode(STATUS_OK);
  res.statusMessage = "OK";

  std::vector<char> encoded = res.encode();
  std::string result(encoded.begin(), encoded.end());

  cr_assert(result.find("HTTP/1.0 200 OK") != std::string::npos);
  cr_assert(result.find("server:WebFloof/") != std::string::npos);
}

Test(http10_response, encode_with_body) {
  mon_http::Http10Response res;
  res.setStatusCode(STATUS_OK);
  res.statusMessage = "OK";
  res.body() = "hello world";

  std::vector<char> encoded = res.encode();
  std::string result(encoded.begin(), encoded.end());

  cr_assert(result.find("HTTP/1.0 200 OK") != std::string::npos);
  cr_assert(result.find("server:WebFloof/") != std::string::npos);
  cr_assert(result.find("content-length:11") != std::string::npos);
  cr_assert(result.find("hello world") != std::string::npos);
}

Test(http10_response, encode_with_headers_no_body) {
  mon_http::Http10Response res;
  res.setStatusCode(STATUS_OK);
  res.statusMessage = "OK";
  res.headers().insert("Host", "example.com");
  res.headers().insert("Content-Type", "text/plain");

  std::vector<char> encoded = res.encode();
  std::string result(encoded.begin(), encoded.end());

  cr_assert(result.find("HTTP/1.0 200 OK") != std::string::npos);
  cr_assert(result.find("host:example.com") != std::string::npos);
  cr_assert(result.find("content-type:text/plain") != std::string::npos);
}

Test(http10_response, encode_with_headers_and_body) {
  mon_http::Http10Response res;
  res.setStatusCode(STATUS_Created);
  res.statusMessage = "Created";
  res.body() = "{\"id\": 1}";
  res.headers().insert("Content-Type", "application/json");

  std::vector<char> encoded = res.encode();
  std::string result(encoded.begin(), encoded.end());

  cr_assert(result.find("HTTP/1.0 201 Created") != std::string::npos);
  cr_assert(result.find("content-type:application/json") != std::string::npos);
  cr_assert(result.find("content-length:9") != std::string::npos);
  cr_assert(result.find("{\"id\": 1}") != std::string::npos);
}

Test(http10_response, encode_empty_status_message_throws) {
  mon_http::Http10Response res;
  res.setStatusCode(STATUS_OK);

  cr_assert_throw(res.encode(), std::runtime_error);
}
