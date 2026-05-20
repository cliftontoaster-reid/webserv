#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <stdint.h>

#include <stdexcept>

#include "Value.hpp"

Test(value, roundtrip_string) {
  std::string s("Very Cool String");

  toml98::Value val = toml98::Value::createString(s);
  const std::string* s2 = val.getString();

  cr_assert_eq(*s2, s);
}

Test(value, roundtrip_integer) {
  int64_t n = 42;

  toml98::Value val = toml98::Value::createInteger(n);
  int64_t n2 = val.getInteger();

  cr_assert_eq(n2, n);
}

Test(value, negative_integer) {
  int64_t n = -1;

  toml98::Value val = toml98::Value::createInteger(n);
  int64_t n2 = val.getInteger();

  cr_assert_eq(n2, n);
}

Test(value, integer_min) {
  int64_t n = INT64_MIN;

  toml98::Value val = toml98::Value::createInteger(n);
  int64_t n2 = val.getInteger();

  cr_assert_eq(n2, n);
}

Test(value, integer_max) {
  int64_t n = INT64_MAX;

  toml98::Value val = toml98::Value::createInteger(n);
  int64_t n2 = val.getInteger();

  cr_assert_eq(n2, n);
}

Test(value, roundtrip_float) {
  double f = 3.14159;

  toml98::Value val = toml98::Value::createFloat(f);
  double f2 = val.getFloat();

  cr_assert_eq(f2, f);
}

Test(value, roundtrip_boolean) {
  bool b = true;

  toml98::Value val = toml98::Value::createBoolean(b);
  bool b2 = val.getBoolean();

  cr_assert_eq(b2, b);
}

Test(value, roundtrip_array) {
  std::vector<toml98::Value> arr;
  arr.push_back(toml98::Value::createInteger(1));
  arr.push_back(toml98::Value::createInteger(2));
  arr.push_back(toml98::Value::createInteger(3));

  toml98::Value val = toml98::Value::createArray(arr);
  const std::vector<toml98::Value>* arr2 = val.getArray();

  cr_assert_eq(arr2->size(), 3);
  cr_assert(arr2->at(0) == toml98::Value::createInteger(1));
  cr_assert(arr2->at(1) == toml98::Value::createInteger(2));
  cr_assert(arr2->at(2) == toml98::Value::createInteger(3));
}

Test(value, roundtrip_table) {
  std::map<std::string, toml98::Value> table;
  table.insert(std::make_pair(std::string("name"),
                              toml98::Value::createString("toml98")));
  table.insert(
      std::make_pair(std::string("version"), toml98::Value::createInteger(1)));

  toml98::Value val = toml98::Value::createTable(table);
  const std::map<std::string, toml98::Value>* table2 = val.getTable();

  cr_assert_eq(table2->size(), 2);
  cr_assert(table2->at("name") == toml98::Value::createString("toml98"));
  cr_assert(table2->at("version") == toml98::Value::createInteger(1));
}

Test(value, roundtrip_nested) {
  std::map<std::string, toml98::Value> inner;
  inner.insert(
      std::make_pair(std::string("x"), toml98::Value::createFloat(1.5)));

  std::vector<toml98::Value> items;
  items.push_back(toml98::Value::createInteger(10));
  items.push_back(toml98::Value::createTable(inner));

  toml98::Value val = toml98::Value::createArray(items);
  const std::vector<toml98::Value>* out = val.getArray();

  cr_assert_eq(out->size(), 2);
  cr_assert(out->at(0) == toml98::Value::createInteger(10));

  const std::map<std::string, toml98::Value>* inner_out = out->at(1).getTable();
  cr_assert_eq(inner_out->size(), 1);
  cr_assert(inner_out->at("x") == toml98::Value::createFloat(1.5));
}

Test(value, wrong_type_string) {
  toml98::Value val = toml98::Value::createString("hello");

  cr_assert_throw(val.getInteger(), std::runtime_error);
  cr_assert_throw(val.getFloat(), std::runtime_error);
  cr_assert_throw(val.getBoolean(), std::runtime_error);
  cr_assert_throw(val.getArray(), std::runtime_error);
  cr_assert_throw(val.getTable(), std::runtime_error);
}

Test(value, wrong_type_integer) {
  toml98::Value val = toml98::Value::createInteger(42);

  cr_assert_throw(val.getString(), std::runtime_error);
  cr_assert_throw(val.getFloat(), std::runtime_error);
  cr_assert_throw(val.getBoolean(), std::runtime_error);
  cr_assert_throw(val.getArray(), std::runtime_error);
  cr_assert_throw(val.getTable(), std::runtime_error);
}

Test(value, wrong_type_float) {
  toml98::Value val = toml98::Value::createFloat(1.5);

  cr_assert_throw(val.getString(), std::runtime_error);
  cr_assert_throw(val.getInteger(), std::runtime_error);
  cr_assert_throw(val.getBoolean(), std::runtime_error);
  cr_assert_throw(val.getArray(), std::runtime_error);
  cr_assert_throw(val.getTable(), std::runtime_error);
}

Test(value, wrong_type_boolean) {
  toml98::Value val = toml98::Value::createBoolean(true);

  cr_assert_throw(val.getString(), std::runtime_error);
  cr_assert_throw(val.getInteger(), std::runtime_error);
  cr_assert_throw(val.getFloat(), std::runtime_error);
  cr_assert_throw(val.getArray(), std::runtime_error);
  cr_assert_throw(val.getTable(), std::runtime_error);
}

Test(value, wrong_type_array) {
  std::vector<toml98::Value> arr;
  arr.push_back(toml98::Value::createInteger(1));
  toml98::Value val = toml98::Value::createArray(arr);

  cr_assert_throw(val.getString(), std::runtime_error);
  cr_assert_throw(val.getInteger(), std::runtime_error);
  cr_assert_throw(val.getFloat(), std::runtime_error);
  cr_assert_throw(val.getBoolean(), std::runtime_error);
  cr_assert_throw(val.getTable(), std::runtime_error);
}

Test(value, wrong_type_table) {
  std::map<std::string, toml98::Value> table;
  toml98::Value val = toml98::Value::createTable(table);

  cr_assert_throw(val.getString(), std::runtime_error);
  cr_assert_throw(val.getInteger(), std::runtime_error);
  cr_assert_throw(val.getFloat(), std::runtime_error);
  cr_assert_throw(val.getBoolean(), std::runtime_error);
  cr_assert_throw(val.getArray(), std::runtime_error);
}
