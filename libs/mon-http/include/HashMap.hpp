
#ifndef MON_HTTP_INCLUDE_HASHMAP_HTTP
#define MON_HTTP_INCLUDE_HASHMAP_HTTP

#include <sys/types.h>

#include <vector>

namespace mon_http {

template <typename Key, typename T>
class HashMap {
 public:
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
  std::vector<Entry> _store;

  u_int32_t hashKey(Key key);
  void resize(u_int64_t newSize);
};

}  // namespace mon_http

#endif