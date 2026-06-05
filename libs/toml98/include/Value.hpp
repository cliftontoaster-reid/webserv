
#ifndef TOML98_INCLUDE_VALUE_HPP
#define TOML98_INCLUDE_VALUE_HPP

#include <stdint.h>

#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace toml98 {

enum ValueType {
  ValueString,
  ValueInteger,
  ValueFloat,
  ValueBoolean,
  ValueArray,
  ValueTable,
};

class Parser;

struct PathPart {
  enum Type {
    PathPartKey,
    PathPartIndex,
  };

  Type type;
  std::string key;
  std::size_t index;

  static PathPart makeKey(const std::string& val) {
    PathPart part;
    part.type = PathPartKey;
    part.key = val;
    part.index = 0;
    return part;
  }

  static PathPart makeIndex(std::size_t val) {
    PathPart part;
    part.type = PathPartIndex;
    part.key = "";
    part.index = val;
    return part;
  }

  bool operator<(const PathPart& other) const;
};

class Value {
  friend class Parser;

 public:
  static Value createString(const std::string& val);
  static Value createInteger(int64_t val);
  static Value createFloat(double val);
  static Value createBoolean(bool val);
  static Value createArray(const std::vector<Value>& val);
  static Value createTable(const std::map<std::string, Value>& val);

  ValueType type() const;

  const std::string* getString() const;
  int64_t getInteger() const;
  double getFloat() const;
  bool getBoolean() const;
  const std::vector<Value>* getArray() const;
  const std::map<std::string, Value>* getTable() const;

  Value(const Value& other);
  Value& operator=(const Value& other);
  ~Value();

  bool operator==(const Value& other) const;
  bool operator!=(const Value& other) const;

  const Value& get(const std::vector<PathPart>& path) const;
  bool has(const std::vector<PathPart>& path) const;
  bool has(const PathPart& part) const;

 private:
  Value(ValueType type, void* val);
  Value(ValueType type, uint64_t val);
  Value();

  ValueType _type;
  union {
    void* _ptr;
    uint64_t _nbr;
  };

  const Value& get_direct_child(const std::string& key) const;
  Value& get_direct_child_mut(const std::string& key);

  std::vector<Value>* getArrayMut();
  std::map<std::string, Value>* getTableMut();
  Value& get_mut(const std::vector<PathPart>& path);

  void insertOrDie(const std::vector<PathPart>& path, const Value& value);
};

std::ostream& operator<<(std::ostream& ost, const Value& val);

}  // namespace toml98

#endif
