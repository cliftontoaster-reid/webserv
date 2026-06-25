
#include <sys/types.h>

#include <cstring>
#include <fstream>
#include <ios>

#define MURMUR_SCRAMBLE_A 0xcc9e2d51
#define MURMUR_SCRAMBLE_B 0x1b873593
#define MURMUR_SCRAMBLE_SHIFT_L 15
#define MURMUR_SCRAMBLE_SHIFT_R 17

#define MURMUR_HASH_A 0xe6546b64
#define MURMUR_HASH_B 0x85ebca6b
#define MURMUR_HASH_C 0xc2b2ae35
#define MURMUR_SHIFT_A 5
#define MURMUR_SHIFT_B 8
#define MURMUR_SHIFT_C 13
#define MURMUR_SHIFT_D 16
#define MURMUR_SHIFT_E 19

namespace mon_http {

// https://en.wikipedia.org/wiki/MurmurHash
inline u_int32_t murmur_32_scramble(u_int32_t nbr) {
  nbr *= MURMUR_SCRAMBLE_A;
  nbr = (nbr << MURMUR_SCRAMBLE_SHIFT_L) | (nbr >> MURMUR_SCRAMBLE_SHIFT_R);
  nbr *= MURMUR_SCRAMBLE_B;
  return nbr;
}

template <typename T>
static inline u_int32_t murmurHash(const T& key, u_int32_t seed) {
  u_int32_t hash = seed;
  u_int32_t nbr = 0;

  // NOLINTNEXTLINE
  const u_int8_t* data = reinterpret_cast<const u_int8_t*>(key.data());
  const u_int8_t* end = data + (key.length() & ~3);

  for (const u_int8_t* ptr = data; ptr < end; ptr += sizeof(u_int32_t)) {
    std::memcpy(&nbr, ptr, sizeof(u_int32_t));
    hash ^= murmur_32_scramble(nbr);
    hash = (hash << MURMUR_SHIFT_C) | (hash >> MURMUR_SHIFT_E);
    hash = (hash * MURMUR_SHIFT_A) + MURMUR_HASH_A;
  }

  nbr = 0;
  const u_int8_t* tail = end;
  for (size_t i = key.length() & 3; i != 0; i--) {
    nbr <<= MURMUR_SHIFT_B;
    nbr |= tail[i - 1];
  }

  hash ^= murmur_32_scramble(nbr);
  hash ^= static_cast<u_int32_t>(key.length());
  hash ^= hash >> MURMUR_SHIFT_D;
  hash *= MURMUR_HASH_B;
  hash ^= hash >> MURMUR_SHIFT_C;
  hash *= MURMUR_HASH_C;
  hash ^= hash >> MURMUR_SHIFT_D;

  return hash;
}

inline u_int32_t random() {
  static unsigned char buf[2048];
  static size_t pos = sizeof(buf);
  if (pos + sizeof(u_int32_t) > sizeof(buf)) {
    std::ifstream file("/dev/urandom", std::ios::binary);
    file.read(reinterpret_cast<char*>(buf), sizeof(buf));
    pos = 0;
  }
  u_int32_t val;
  std::memcpy(&val, buf + pos, sizeof(val));
  pos += sizeof(val);
  return val;
}

template <typename Key, typename T>
HashMap<Key, T>::HashMap(size_t initial_size)
    : _store(initial_size),
      _size(0),
      _load_factor_threshold(HASHMAP_DEFAULT_MAX_LOAD),
      _seed(random()) {}

template <typename Key, typename T>
HashMap<Key, T>::HashMap(const HashMap<Key, T>& other)
    : _store(other._store),
      _size(other._size),
      _load_factor_threshold(other._load_factor_threshold),
      _seed(other._seed) {}

template <typename Key, typename T>
HashMap<Key, T>& HashMap<Key, T>::operator=(const HashMap<Key, T>& other) {
  if (this != &other) {
    _store = other._store;
    _size = other._size;
    _load_factor_threshold = other._load_factor_threshold;
    _seed = other._seed;
  }
  return *this;
}

template <typename Key, typename T>
HashMap<Key, T>::~HashMap() {}

template <typename Key, typename T>
bool HashMap<Key, T>::contains(const Key& key) const {
  u_int32_t hash = murmurHash(key, _seed);
  u_int32_t idx = hash & (_store.size() - 1);

  for (size_t i = 0; i < _store[idx].size(); ++i) {
    if (_store[idx][i].occupied && _store[idx][i].key == key) {
      return true;
    }
  }
  return false;
}

template <typename Key, typename T>
void HashMap<Key, T>::clear() {
  for (size_t i = 0; i < _store.size(); ++i) {
    _store[i].clear();
  }
  _size = 0;
}

template <typename Key, typename T>
const T& HashMap<Key, T>::at(const Key& key) const {
  u_int32_t hash = murmurHash(key, _seed);
  u_int32_t idx = hash & (_store.size() - 1);

  for (size_t i = 0; i < _store[idx].size(); ++i) {
    if (_store[idx][i].occupied && _store[idx][i].key == key) {
      return _store[idx][i].value;
    }
  }

  throw std::out_of_range("Key not found");
}

template <typename Key, typename T>
T& HashMap<Key, T>::at(const Key& key) {
  u_int32_t hash = murmurHash(key, _seed);
  u_int32_t idx = hash & (_store.size() - 1);

  for (size_t i = 0; i < _store[idx].size(); ++i) {
    if (_store[idx][i].occupied && _store[idx][i].key == key) {
      return _store[idx][i].value;
    }
  }

  throw std::out_of_range("Key not found");
}

template <typename Key, typename T>
T& HashMap<Key, T>::operator[](const Key& key) {
  u_int32_t hash = murmurHash(key, _seed);
  u_int32_t idx = hash & (_store.size() - 1);

  for (size_t i = 0; i < _store[idx].size(); ++i) {
    if (_store[idx][i].occupied && _store[idx][i].key == key) {
      return _store[idx][i].value;
    }
  }

  _store[idx].push_back(Entry());
  _store[idx].back().key = key;
  _store[idx].back().occupied = true;

  _size++;
  if ((static_cast<float>(_size) / _store.size()) > _load_factor_threshold) {
    resize(_store.size() * 2);
    hash = murmurHash(key, _seed);
    idx = hash & (_store.size() - 1);
    for (size_t i = 0; i < _store[idx].size(); ++i) {
      if (_store[idx][i].occupied && _store[idx][i].key == key) {
        return _store[idx][i].value;
      }
    }
  }

  return _store[idx].back().value;
}

template <typename Key, typename T>
void HashMap<Key, T>::insert(const Key& key, const T& value) {
  u_int32_t hash = murmurHash(key, _seed);
  u_int32_t idx = hash & (_store.size() - 1);

  for (size_t i = 0; i < _store[idx].size(); ++i) {
    if (_store[idx][i].occupied && _store[idx][i].key == key) {
      _store[idx][i].value = value;
      return;
    }
  }

  Entry etr;
  etr.key = key;
  etr.value = value;
  etr.occupied = true;
  _store[idx].push_back(etr);

  _size++;
  if ((static_cast<float>(_size) / _store.size()) > _load_factor_threshold) {
    resize(_store.size() * 2);
  }
}

template <typename Key, typename T>
void HashMap<Key, T>::remove(const Key& key) {
  u_int32_t hash = murmurHash(key, _seed);
  u_int32_t idx = hash & (_store.size() - 1);

  for (size_t i = 0; i < _store[idx].size(); ++i) {
    if (_store[idx][i].occupied && _store[idx][i].key == key) {
      _store[idx].erase(_store[idx].begin() + i);
      _size--;
      return;
    }
  }
}

template <typename Key, typename T>
void HashMap<Key, T>::resize(u_int64_t newSize) {
  std::vector<std::vector<Entry> > oldStore;
  oldStore.swap(_store);
  _store.resize(newSize);
  for (size_t i = 0; i < oldStore.size(); ++i) {
    for (size_t j = 0; j < oldStore[i].size(); ++j) {
      if (oldStore[i][j].occupied) {
        insert(oldStore[i][j].key, oldStore[i][j].value);
      }
    }
  }
}

template <typename Key, typename T>
template <typename F>
void HashMap<Key, T>::iter(F func) {
  for (size_t i = 0; i < _store.size(); ++i) {
    for (size_t j = 0; j < _store[i].size(); ++j) {
      if (_store[i][j].occupied) {
        func(_store[i][j].key, _store[i][j].value);
      }
    }
  }
}

}  // namespace mon_http
