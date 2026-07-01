
#ifndef MON_HTTP_INCLUDE_HASHMAP_HPP
#define MON_HTTP_INCLUDE_HASHMAP_HPP

#include <sys/types.h>

#include <cstddef>
#include <vector>

#define HASHMAP_DEFAULT_MAX_LOAD 0.75
#define HASHMAP_DEFAULT_BUCKETS 16

namespace mon_http {

template <typename Key, typename T>
class HashMap {
 public:
  explicit HashMap(size_t initial_size = HASHMAP_DEFAULT_BUCKETS);
  HashMap(const HashMap& other);
  HashMap& operator=(const HashMap& other);
  ~HashMap();

  struct Entry {
    Key key;
    T value;
    bool occupied;
  };

  T& at(const Key& key);
  const T& at(const Key& key) const;
  T& operator[](const Key& key);

  bool contains(const Key& key) const;

  void clear();

  void insert(const Key& key, const T& value);
  void remove(const Key& key);

  template <typename F>
  void iter(F func);

 private:
  std::vector<std::vector<Entry> > _store;
  size_t _size;
  float _load_factor_threshold;
  u_int32_t _seed;

  void resize(u_int64_t newSize);
};

}  // namespace mon_http

#include "HashMap.tpp"

#endif
