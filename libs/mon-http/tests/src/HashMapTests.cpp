#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

#include <stdexcept>
#include <string>

#include "HashMap.hpp"

Test(hash_map, default_construction) {
  mon_http::HashMap<std::string, int> map;
}

Test(hash_map, custom_initial_size) {
  mon_http::HashMap<std::string, int> map(32);
}

Test(hash_map, insert_and_at) {
  mon_http::HashMap<std::string, int> map;

  map.insert("one", 1);
  map.insert("two", 2);
  map.insert("three", 3);

  cr_assert_eq(map.at("one"), 1);
  cr_assert_eq(map.at("two"), 2);
  cr_assert_eq(map.at("three"), 3);
}

Test(hash_map, at_throws_if_not_found) {
  mon_http::HashMap<std::string, int> map;

  cr_assert_throw(map.at("nonexistent"), std::out_of_range);
}

Test(hash_map, operator_bracket_inserts_default) {
  mon_http::HashMap<std::string, int> map;

  int& val = map["new_key"];
  cr_assert_eq(val, 0);
}

Test(hash_map, operator_bracket_returns_reference) {
  mon_http::HashMap<std::string, int> map;

  map["key"] = 42;
  cr_assert_eq(map["key"], 42);
}

Test(hash_map, insert_overwrites_existing_key) {
  mon_http::HashMap<std::string, int> map;

  map.insert("key", 1);
  map.insert("key", 2);

  cr_assert_eq(map.at("key"), 2);
}

Test(hash_map, clear_removes_all) {
  mon_http::HashMap<std::string, int> map;

  map.insert("a", 1);
  map.insert("b", 2);
  map.clear();

  cr_assert_throw(map.at("a"), std::out_of_range);
  cr_assert_throw(map.at("b"), std::out_of_range);
}

Test(hash_map, remove_existing_key) {
  mon_http::HashMap<std::string, int> map;

  map.insert("key", 42);
  map.remove("key");

  cr_assert_throw(map.at("key"), std::out_of_range);
}

Test(hash_map, remove_nonexistent_key) {
  mon_http::HashMap<std::string, int> map;

  map.remove("nonexistent");
}

Test(hash_map, copy_construction) {
  mon_http::HashMap<std::string, int> map;
  map.insert("a", 1);
  map.insert("b", 2);

  mon_http::HashMap<std::string, int> copy(map);
  cr_assert_eq(copy.at("a"), 1);
  cr_assert_eq(copy.at("b"), 2);

  copy.insert("c", 3);
  cr_assert_throw(map.at("c"), std::out_of_range);
}

Test(hash_map, assignment_operator) {
  mon_http::HashMap<std::string, int> map;
  map.insert("x", 10);

  mon_http::HashMap<std::string, int> copy;
  copy = map;
  cr_assert_eq(copy.at("x"), 10);

  copy.insert("y", 20);
  cr_assert_throw(map.at("y"), std::out_of_range);
}

Test(hash_map, resize_triggers_on_load) {
  mon_http::HashMap<std::string, int> map(4);

  for (int i = 0; i < 100; i++) {
    map.insert("key" + std::to_string(i), i);
  }

  for (int i = 0; i < 100; i++) {
    cr_assert_eq(map.at("key" + std::to_string(i)), i);
  }
}
