#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <string>

#include "HttpMethod.hpp"

Test(http_method, default_construction) {
  mon_http::HttpMethod method;
}

Test(http_method, construction_with_type) {
  mon_http::HttpMethod get(mon_http::HttpMethod::HttpMethodGet);
  cr_assert_eq(get.getType(), mon_http::HttpMethod::HttpMethodGet);

  mon_http::HttpMethod post(mon_http::HttpMethod::HttpMethodPost);
  cr_assert_eq(post.getType(), mon_http::HttpMethod::HttpMethodPost);

  mon_http::HttpMethod unknown(mon_http::HttpMethod::HttpMethodUnknown);
  cr_assert_eq(unknown.getType(), mon_http::HttpMethod::HttpMethodUnknown);
}

Test(http_method, copy_construction) {
  mon_http::HttpMethod original(mon_http::HttpMethod::HttpMethodPost);
  mon_http::HttpMethod copy(original);

  cr_assert_eq(copy.getType(), mon_http::HttpMethod::HttpMethodPost);
}

Test(http_method, assignment_operator) {
  mon_http::HttpMethod original(mon_http::HttpMethod::HttpMethodDelete);
  mon_http::HttpMethod copy;
  copy = original;

  cr_assert_eq(copy.getType(), mon_http::HttpMethod::HttpMethodDelete);
}

Test(http_method, assignment_independent) {
  mon_http::HttpMethod original(mon_http::HttpMethod::HttpMethodPut);
  mon_http::HttpMethod assigned;
  assigned = original;
  cr_assert_eq(assigned.getType(), mon_http::HttpMethod::HttpMethodPut);
}

Test(http_method, get_type) {
  mon_http::HttpMethod method(mon_http::HttpMethod::HttpMethodHead);
  cr_assert_eq(method.getType(), mon_http::HttpMethod::HttpMethodHead);
}

Test(http_method, from_string_get) {
  mon_http::HttpMethod method = mon_http::HttpMethod::fromString("GET");
  cr_assert_eq(method.getType(), mon_http::HttpMethod::HttpMethodGet);
}

Test(http_method, from_string_head) {
  mon_http::HttpMethod method = mon_http::HttpMethod::fromString("HEAD");
  cr_assert_eq(method.getType(), mon_http::HttpMethod::HttpMethodHead);
}

Test(http_method, from_string_post) {
  mon_http::HttpMethod method = mon_http::HttpMethod::fromString("POST");
  cr_assert_eq(method.getType(), mon_http::HttpMethod::HttpMethodPost);
}

Test(http_method, from_string_put) {
  mon_http::HttpMethod method = mon_http::HttpMethod::fromString("PUT");
  cr_assert_eq(method.getType(), mon_http::HttpMethod::HttpMethodPut);
}

Test(http_method, from_string_delete) {
  mon_http::HttpMethod method = mon_http::HttpMethod::fromString("DELETE");
  cr_assert_eq(method.getType(), mon_http::HttpMethod::HttpMethodDelete);
}

Test(http_method, from_string_connect) {
  mon_http::HttpMethod method = mon_http::HttpMethod::fromString("CONNECT");
  cr_assert_eq(method.getType(), mon_http::HttpMethod::HttpMethodConnect);
}

Test(http_method, from_string_options) {
  mon_http::HttpMethod method = mon_http::HttpMethod::fromString("OPTIONS");
  cr_assert_eq(method.getType(), mon_http::HttpMethod::HttpMethodOptions);
}

Test(http_method, from_string_trace) {
  mon_http::HttpMethod method = mon_http::HttpMethod::fromString("TRACE");
  cr_assert_eq(method.getType(), mon_http::HttpMethod::HttpMethodTrace);
}

Test(http_method, from_string_patch) {
  mon_http::HttpMethod method = mon_http::HttpMethod::fromString("PATCH");
  cr_assert_eq(method.getType(), mon_http::HttpMethod::HttpMethodPatch);
}

Test(http_method, from_string_unknown) {
  mon_http::HttpMethod method = mon_http::HttpMethod::fromString("INVALID");
  cr_assert_eq(method.getType(), mon_http::HttpMethod::HttpMethodUnknown);
}

Test(http_method, from_string_case_sensitive) {
  mon_http::HttpMethod lower = mon_http::HttpMethod::fromString("get");
  cr_assert_eq(lower.getType(), mon_http::HttpMethod::HttpMethodUnknown);

  mon_http::HttpMethod mixed = mon_http::HttpMethod::fromString("Get");
  cr_assert_eq(mixed.getType(), mon_http::HttpMethod::HttpMethodUnknown);
}

Test(http_method, to_string) {
  mon_http::HttpMethod get(mon_http::HttpMethod::HttpMethodGet);
  cr_assert_eq(get.toString(), "GET");

  mon_http::HttpMethod head(mon_http::HttpMethod::HttpMethodHead);
  cr_assert_eq(head.toString(), "HEAD");

  mon_http::HttpMethod post(mon_http::HttpMethod::HttpMethodPost);
  cr_assert_eq(post.toString(), "POST");

  mon_http::HttpMethod put(mon_http::HttpMethod::HttpMethodPut);
  cr_assert_eq(put.toString(), "PUT");

  mon_http::HttpMethod del(mon_http::HttpMethod::HttpMethodDelete);
  cr_assert_eq(del.toString(), "DELETE");

  mon_http::HttpMethod connect(mon_http::HttpMethod::HttpMethodConnect);
  cr_assert_eq(connect.toString(), "CONNECT");

  mon_http::HttpMethod options(mon_http::HttpMethod::HttpMethodOptions);
  cr_assert_eq(options.toString(), "OPTIONS");

  mon_http::HttpMethod trace(mon_http::HttpMethod::HttpMethodTrace);
  cr_assert_eq(trace.toString(), "TRACE");

  mon_http::HttpMethod patch(mon_http::HttpMethod::HttpMethodPatch);
  cr_assert_eq(patch.toString(), "PATCH");
}

Test(http_method, to_string_unknown) {
  mon_http::HttpMethod method;
  cr_assert_eq(method.toString(), "UNKNOWN");
}

Test(http_method, roundtrip) {
  const char* methods[] = {
    "GET", "HEAD", "POST", "PUT", "DELETE",
    "CONNECT", "OPTIONS", "TRACE", "PATCH",
  };

  for (size_t i = 0; i < sizeof(methods) / sizeof(methods[0]); i++) {
    mon_http::HttpMethod method = mon_http::HttpMethod::fromString(methods[i]);
    cr_assert_eq(method.toString(), methods[i]);
  }
}
