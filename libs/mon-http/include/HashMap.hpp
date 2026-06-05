
#ifndef MON_HTTP_INCLUDE_HASHMAP_HTTP
#define MON_HTTP_INCLUDE_HASHMAP_HTTP

#include <sys/types.h>

#include <cstddef>
#include <vector>

namespace mon_http {

template <typename Key, typename T>
class HashMap {
 public:
  explicit HashMap<Key, T>(size_t initial_size = 16);
  HashMap<Key, T>(const HashMap<Key, T>& other);
  HashMap<Key, T>& operator=(const HashMap<Key, T>& other);
  ~HashMap<Key, T>();

  struct Entry {
    Key key;
    T value;
    bool occupied;
  };

  T& at(const Key& key);
  T& operator[](const Key& key);

  void clear();

  void insert(const Key& key, const T& value);
  void remove(const Key& key);

 private:
  std::vector<std::vector<Entry> > _store;
  size_t _size;
  float _load_factor_threshold;
  u_int32_t _seed;

  void resize(u_int64_t newSize);
};

}  // namespace mon_http

#endif
