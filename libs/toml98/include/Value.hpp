
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

class Value {
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

 private:
  Value(ValueType type, void* val);
  Value(ValueType type, uint64_t val);
  Value();

  ValueType _type;
  union {
    void* _ptr;
    uint64_t _nbr;
  };
};

std::ostream& operator<<(std::ostream& ost, const Value& val);

}  // namespace toml98

#endif