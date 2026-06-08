#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <vector>

#include "HttpVersion.hpp"

Test(http_version, default_construction) {
  mon_http::HttpVersion version;
}

Test(http_version, construction_with_type) {
  mon_http::HttpVersion v11(mon_http::HttpVersion::HttpVersion1_1);
  cr_assert_eq(v11.value, mon_http::HttpVersion::HttpVersion1_1);

  mon_http::HttpVersion unknown(mon_http::HttpVersion::HttpVersionUnknown);
  cr_assert_eq(unknown.value, mon_http::HttpVersion::HttpVersionUnknown);
}

Test(http_version, copy_construction) {
  mon_http::HttpVersion original(mon_http::HttpVersion::HttpVersion1_0);
  mon_http::HttpVersion copy(original);

  cr_assert_eq(copy.value, mon_http::HttpVersion::HttpVersion1_0);
}

Test(http_version, assignment_operator) {
  mon_http::HttpVersion original(mon_http::HttpVersion::HttpVersion2_0);
  mon_http::HttpVersion copy;
  copy = original;

  cr_assert_eq(copy.value, mon_http::HttpVersion::HttpVersion2_0);
}

Test(http_version, to_string) {
  mon_http::HttpVersion unknown(mon_http::HttpVersion::HttpVersionUnknown);
  cr_assert_eq(std::string(unknown.toString()), "unknown HTTP version");

  mon_http::HttpVersion v09(mon_http::HttpVersion::HttpVersion0_9);
  cr_assert_eq(std::string(v09.toString()), "HTTP/0.9");

  mon_http::HttpVersion v10(mon_http::HttpVersion::HttpVersion1_0);
  cr_assert_eq(std::string(v10.toString()), "HTTP/1.0");

  mon_http::HttpVersion v11(mon_http::HttpVersion::HttpVersion1_1);
  cr_assert_eq(std::string(v11.toString()), "HTTP/1.1");

  mon_http::HttpVersion v20(mon_http::HttpVersion::HttpVersion2_0);
  cr_assert_eq(std::string(v20.toString()), "HTTP/2.0");
}

Test(http_version, sniff_http_2_0) {
  std::vector<char> data = {'P', 'R', 'I', ' ', '*', ' ', 'H', 'T', 'T', 'P',
                            '/', '2', '.', '0', '\r', '\n', '\r', '\n', 'S',
                            'M', '\r', '\n', '\r', '\n'};

  mon_http::HttpVersion version = mon_http::HttpVersion::sniffHttpVersion(data);
  cr_assert_eq(version.value, mon_http::HttpVersion::HttpVersion2_0);
}

Test(http_version, sniff_http_1_1_request) {
  std::string req = "GET /index.html HTTP/1.1\r\n";
  std::vector<char> data(req.begin(), req.end());

  mon_http::HttpVersion version = mon_http::HttpVersion::sniffHttpVersion(data);
  cr_assert_eq(version.value, mon_http::HttpVersion::HttpVersion1_1);
}

Test(http_version, sniff_http_1_0_request) {
  std::string req = "GET /index.html HTTP/1.0\r\n";
  std::vector<char> data(req.begin(), req.end());

  mon_http::HttpVersion version = mon_http::HttpVersion::sniffHttpVersion(data);
  cr_assert_eq(version.value, mon_http::HttpVersion::HttpVersion1_0);
}

Test(http_version, sniff_http_0_9) {
  std::string req = "GET /index.html\r\n";
  std::vector<char> data(req.begin(), req.end());

  mon_http::HttpVersion version = mon_http::HttpVersion::sniffHttpVersion(data);
  cr_assert_eq(version.value, mon_http::HttpVersion::HttpVersion0_9);
}

Test(http_version, sniff_unknown) {
  std::string garbage = "foobarbaz\r\n";
  std::vector<char> data(garbage.begin(), garbage.end());

  mon_http::HttpVersion version = mon_http::HttpVersion::sniffHttpVersion(data);
  cr_assert_eq(version.value, mon_http::HttpVersion::HttpVersionUnknown);
}

Test(http_version, sniff_empty) {
  std::vector<char> data;

  mon_http::HttpVersion version = mon_http::HttpVersion::sniffHttpVersion(data);
  cr_assert_eq(version.value, mon_http::HttpVersion::HttpVersionUnknown);
}

Test(http_version, sniff_partial_http_prefix) {
  std::string partial = "HTTP/1.";
  std::vector<char> data(partial.begin(), partial.end());

  mon_http::HttpVersion version = mon_http::HttpVersion::sniffHttpVersion(data);
  cr_assert_eq(version.value, mon_http::HttpVersion::HttpVersionUnknown);
}

Test(http_version, sniff_no_crlf) {
  std::string no_crlf = "GET / HTTP/1.1";
  std::vector<char> data(no_crlf.begin(), no_crlf.end());

  mon_http::HttpVersion version = mon_http::HttpVersion::sniffHttpVersion(data);
  cr_assert_eq(version.value, mon_http::HttpVersion::HttpVersionUnknown);
}
