#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stdexcept>
#include <string>

#include "HeaderMap.hpp"

Test(header_map, default_construction) { mon_http::HeaderMap map; }

Test(header_map, insert_and_at) {
  mon_http::HeaderMap map;

  map.insert("Host", "example.com");
  map.insert("Content-Type", "text/html");

  cr_assert_eq(map.at("Host"), "example.com");
  cr_assert_eq(map.at("Content-Type"), "text/html");
}

Test(header_map, at_throws_if_not_found) {
  mon_http::HeaderMap map;

  cr_assert_throw(map.at("Nonexistent"), std::out_of_range);
}

Test(header_map, operator_bracket_inserts_default) {
  mon_http::HeaderMap map;

  std::string& val = map["X-Custom"];
  cr_assert_eq(val, "");
}

Test(header_map, operator_bracket_returns_reference) {
  mon_http::HeaderMap map;

  map["X-Custom"] = "my-value";
  cr_assert_eq(map["X-Custom"], "my-value");
}

Test(header_map, case_insensitive_insert_then_at) {
  mon_http::HeaderMap map;

  map.insert("Content-Type", "application/json");
  cr_assert_eq(map.at("content-type"), "application/json");
}

Test(header_map, case_insensitive_at_with_different_case) {
  mon_http::HeaderMap map;

  map.insert("CONTENT-TYPE", "text/plain");
  cr_assert_eq(map.at("Content-Type"), "text/plain");
}

Test(header_map, operator_bracket_case_insensitive) {
  mon_http::HeaderMap map;

  map["CONTENT-TYPE"] = "application/json";
  cr_assert_eq(map["content-type"], "application/json");
}

Test(header_map, insert_overwrites_case_insensitive) {
  mon_http::HeaderMap map;

  map.insert("Foo", "first");
  map.insert("FOO", "second");

  cr_assert_eq(map.at("Foo"), "second");
}

Test(header_map, clear_removes_all) {
  mon_http::HeaderMap map;

  map.insert("A", "1");
  map.insert("B", "2");
  map.clear();

  cr_assert_throw(map.at("A"), std::out_of_range);
  cr_assert_throw(map.at("b"), std::out_of_range);
}

Test(header_map, remove_headers) {
  mon_http::HeaderMap map;

  map.insert("Host", "example.com");
  map.remove("Host");

  cr_assert_throw(map.at("Host"), std::out_of_range);
}

Test(header_map, remove_case_insensitive) {
  mon_http::HeaderMap map;

  map.insert("Content-Type", "text/html");
  map.remove("content-type");

  cr_assert_throw(map.at("Content-Type"), std::out_of_range);
}

Test(header_map, copy_construction) {
  mon_http::HeaderMap map;
  map.insert("Host", "example.com");

  mon_http::HeaderMap copy(map);
  cr_assert_eq(copy.at("Host"), "example.com");

  copy.insert("X-Custom", "value");
  cr_assert_throw(map.at("X-Custom"), std::out_of_range);
}

Test(header_map, assignment_operator) {
  mon_http::HeaderMap map;
  map.insert("Host", "example.com");

  mon_http::HeaderMap copy;
  copy = map;
  cr_assert_eq(copy.at("Host"), "example.com");

  copy.insert("X-New", "val");
  cr_assert_throw(map.at("X-New"), std::out_of_range);
}

Test(header_map, multiple_headers) {
  mon_http::HeaderMap map;

  map.insert("Host", "example.com");
  map.insert("User-Agent", "curl/8.0");
  map.insert("Accept", "*/*");
  map.insert("Content-Length", "42");

  cr_assert_eq(map.at("HOST"), "example.com");
  cr_assert_eq(map.at("user-agent"), "curl/8.0");
  cr_assert_eq(map.at("ACCEPT"), "*/*");
  cr_assert_eq(map.at("content-length"), "42");
}
