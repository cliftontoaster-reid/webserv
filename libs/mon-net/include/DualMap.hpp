#ifndef MON_NET_INCLUDE_DUALMAP_HTTP
#define MON_NET_INCLUDE_DUALMAP_HTTP

#include <map>
#include <stdexcept>

namespace mon_net {

template <typename A, typename B>
class DualMap {
 public:
  DualMap() {}
  DualMap(const DualMap& other) : north(other.north), south(other.south) {}
  DualMap& operator=(const DualMap& other) {
    if (this != &other) {
      this->north = other.north;
      this->south = other.south;
    }
    return *this;
  }
  ~DualMap() {}

  // Returns true if insertion was successful
  bool insert(const A& a, const B& b) {
    if (north.count(a) || south.count(b)) {
      return false;
    }

    north[a] = b;
    south[b] = a;
    return true;
  }

  // Lookup B by A
  const B& get_by_a(const A& a) const {
    typename std::map<A, B>::const_iterator iter = north.find(a);
    if (iter == north.end()) {
      throw std::out_of_range("Key A not found");
    }
    return iter->second;
  }
  bool has_a(const A& a) const {
    typename std::map<A, B>::const_iterator iter = north.find(a);
    return static_cast<bool>(iter != north.end());
  }

  // Lookup A by B
  const A& get_by_b(const B& b) const {
    typename std::map<B, A>::const_iterator iter = south.find(b);
    if (iter == south.end()) {
      throw std::out_of_range("Key B not found");
    }
    return iter->second;
  }
  bool has_b(const B& b) const {
    typename std::map<B, A>::const_iterator iter = south.find(b);
    return static_cast<bool>(iter != south.end());
  }

  void remove_by_a(const A& a) {
    typename std::map<A, B>::iterator iter = north.find(a);
    if (iter != north.end()) {
      south.erase(iter->second);
      north.erase(iter);
    }
  }
  void remove_by_b(const B& b) {
    typename std::map<A, B>::iterator iter = south.find(b);
    if (iter != south.end()) {
      north.erase(iter->second);
      south.erase(iter);
    }
  }

  // ... Add size(), clear(), contains() as needed
 private:
  std::map<A, B> north;
  std::map<B, A> south;
};

}  // namespace mon_net

#endif