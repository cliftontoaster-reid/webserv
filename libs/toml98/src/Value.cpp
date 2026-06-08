#include "Value.hpp"

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace toml98 {

Value Value::createString(const std::string& val) {
  std::string* ptr = new std::string(val);
  return Value(ValueString, static_cast<void*>(ptr));
}

Value Value::createInteger(int64_t val) {
  return Value(ValueInteger, static_cast<uint64_t>(val));
}

Value Value::createFloat(double val) {
  uint64_t bits = 0;
  std::memcpy(&bits, &val, sizeof(val));
  return Value(ValueFloat, bits);
}

Value Value::createBoolean(bool val) {
  return Value(ValueBoolean, static_cast<uint64_t>(val));
}

Value Value::createArray(const std::vector<Value>& val) {
  std::vector<Value>* tmp = new std::vector<Value>(val);
  return Value(ValueArray, tmp);
}

Value Value::createTable(const std::map<std::string, Value>& val) {
  std::map<std::string, Value>* tmp = new std::map<std::string, Value>(val);
  return Value(ValueTable, tmp);
}

ValueType Value::type() const { return _type; }

const std::string* Value::getString() const {
  if (_type != ValueString) {
    throw std::runtime_error("Value is not a string");
  }
  return static_cast<const std::string*>(_ptr);
}

int64_t Value::getInteger() const {
  if (_type != ValueInteger) {
    throw std::runtime_error("Value is not an integer");
  }
  return static_cast<int64_t>(_nbr);
}

double Value::getFloat() const {
  if (_type != ValueFloat) {
    throw std::runtime_error("Value is not a float");
  }
  uint64_t bits = _nbr;
  double val = NAN;
  std::memcpy(&val, &bits, sizeof(val));
  return val;
}

bool Value::getBoolean() const {
  if (_type != ValueBoolean) {
    throw std::runtime_error("Value is not a boolean");
  }
  return static_cast<bool>(_nbr);
}

const std::vector<Value>* Value::getArray() const {
  if (_type != ValueArray) {
    throw std::runtime_error("Value is not an array");
  }
  return static_cast<const std::vector<Value>*>(_ptr);
}

const std::map<std::string, Value>* Value::getTable() const {
  if (_type != ValueTable) {
    throw std::runtime_error("Value is not a table");
  }
  return static_cast<const std::map<std::string, Value>*>(_ptr);
}

std::vector<Value>* Value::getArrayMut() {
  if (_type != ValueArray) {
    throw std::runtime_error("Value is not an array");
  }
  return static_cast<std::vector<Value>*>(_ptr);
}

std::map<std::string, Value>* Value::getTableMut() {
  if (_type != ValueTable) {
    throw std::runtime_error("Value is not a table");
  }
  return static_cast<std::map<std::string, Value>*>(_ptr);
}

Value::Value(const Value& other) : _type(other._type), _ptr(NULL) {
  switch (_type) {
    case ValueString: {
      const std::string* src = static_cast<const std::string*>(other._ptr);
      _ptr = static_cast<void*>(new std::string(*src));
      break;
    }
    case ValueArray: {
      const std::vector<Value>* src =
          static_cast<const std::vector<Value>*>(other._ptr);
      _ptr = static_cast<void*>(new std::vector<Value>(*src));
      break;
    }
    case ValueTable: {
      const std::map<std::string, Value>* src =
          static_cast<const std::map<std::string, Value>*>(other._ptr);
      _ptr = static_cast<void*>(new std::map<std::string, Value>(*src));
      break;
    }
    case ValueInteger:
    case ValueFloat:
    case ValueBoolean:
      _nbr = other._nbr;
      break;
  }
}

Value& Value::operator=(const Value& other) {
  if (this != &other) {
    ValueType oldType = _type;
    void* oldPtr = _ptr;

    // Copy new data first (may throw)
    _type = other._type;
    _ptr = NULL;
    switch (_type) {
      case ValueString: {
        const std::string* src = static_cast<const std::string*>(other._ptr);
        _ptr = static_cast<void*>(new std::string(*src));
        break;
      }
      case ValueArray: {
        const std::vector<Value>* src =
            static_cast<const std::vector<Value>*>(other._ptr);
        _ptr = static_cast<void*>(new std::vector<Value>(*src));
        break;
      }
      case ValueTable: {
        const std::map<std::string, Value>* src =
            static_cast<const std::map<std::string, Value>*>(other._ptr);
        _ptr = static_cast<void*>(new std::map<std::string, Value>(*src));
        break;
      }
      case ValueInteger:
      case ValueFloat:
      case ValueBoolean:
        _nbr = other._nbr;
        break;
    }

    // Destroy old data only after successful copy
    switch (oldType) {
      case ValueString: {
        std::string* str = static_cast<std::string*>(oldPtr);
        delete str;
        break;
      }
      case ValueArray: {
        std::vector<Value>* array = static_cast<std::vector<Value>*>(oldPtr);
        delete array;
        break;
      }
      case ValueTable: {
        std::map<std::string, Value>* table =
            static_cast<std::map<std::string, Value>*>(oldPtr);
        delete table;
        break;
      }
      case ValueInteger:
      case ValueFloat:
      case ValueBoolean:
        break;
    }
  }
  return *this;
}

Value::~Value() {
  switch (_type) {
    case ValueString: {
      std::string* str = static_cast<std::string*>(_ptr);
      delete str;
      break;
    }
    case ValueArray: {
      std::vector<Value>* array = static_cast<std::vector<Value>*>(_ptr);
      delete array;
      break;
    }
    case ValueTable: {
      std::map<std::string, Value>* table =
          static_cast<std::map<std::string, Value>*>(_ptr);
      delete table;
      break;
    }
    case ValueInteger:
    case ValueFloat:
    case ValueBoolean:
      break;
  }
}

bool Value::operator==(const Value& other) const {
  if (_type != other._type) {
    return false;
  }
  switch (_type) {
    case ValueString:
      return *static_cast<const std::string*>(_ptr) ==
             *static_cast<const std::string*>(other._ptr);
    case ValueInteger:
      return _nbr == other._nbr;
    case ValueFloat: {
      double floatA = NAN;
      double floatB = NAN;
      std::memcpy(&floatA, &_nbr, sizeof(floatA));
      std::memcpy(&floatB, &other._nbr, sizeof(floatB));
      return floatA == floatB;
    }
    case ValueBoolean:
      return static_cast<bool>(_nbr) == static_cast<bool>(other._nbr);
    case ValueArray:
      return *static_cast<const std::vector<Value>*>(_ptr) ==
             *static_cast<const std::vector<Value>*>(other._ptr);
    case ValueTable:
      return *static_cast<const std::map<std::string, Value>*>(_ptr) ==
             *static_cast<const std::map<std::string, Value>*>(other._ptr);
  }
  return false;
}

bool Value::operator!=(const Value& other) const { return !(*this == other); }

Value::Value(ValueType type, void* val) : _type(type), _ptr(val) {}
Value::Value(ValueType type, uint64_t val) : _type(type), _nbr(val) {}
Value::Value() : _type(ValueString), _ptr() {}

const Value& Value::get(const std::vector<PathPart>& path) const {
  if (path.empty()) {
    throw std::runtime_error("Empty path");
  }

  const Value* cur = this;
  for (std::vector<PathPart>::const_iterator it = path.begin();
       it != path.end(); ++it) {
    if (it->type == PathPart::PathPartKey) {
      cur = &cur->get_direct_child(it->key);
    } else {
      if (cur->_type != ValueArray) {
        throw std::runtime_error("Cannot traverse non-array type");
      }
      const std::vector<Value>* arr = cur->getArray();
      if (it->index >= arr->size()) {
        throw std::runtime_error("Array index out of bounds");
      }
      cur = &arr->at(it->index);
    }
  }
  return *cur;
}

bool Value::has(const std::vector<PathPart>& path) const {
  try {
    (void)get(path);
    return true;
  } catch (const std::exception&) {
    return false;
  }
}
bool Value::has(const PathPart& part) const {
  try {
    std::vector<PathPart> path;
    path.push_back(part);
    (void)get(path);
    return true;
  } catch (const std::exception&) {
    return false;
  }
}

Value& Value::get_mut(const std::vector<PathPart>& path) {
  if (path.empty()) {
    throw std::runtime_error("Empty path");
  }

  Value* cur = this;
  for (std::vector<PathPart>::const_iterator it = path.begin();
       it != path.end(); ++it) {
    if (it->type == PathPart::PathPartKey) {
      cur = &cur->get_direct_child_mut(it->key);
    } else {
      std::vector<Value>* arr = cur->getArrayMut();
      if (it->index >= arr->size()) {
        throw std::runtime_error("Array index out of bounds");
      }
      cur = &arr->at(it->index);
    }
  }
  return *cur;
}

const Value& Value::get_direct_child(const std::string& key) const {
  if (_type == ValueTable) {
    const std::map<std::string, Value>* table = getTable();
    std::map<std::string, Value>::const_iterator iter = table->find(key);

    if (iter == table->end()) {
      throw std::runtime_error("Key not found in table: " + key);
    }
    return iter->second;
  }
  if (_type == ValueArray) {
    long index = std::strtol(key.c_str(), NULL, 10);
    const std::vector<Value>* arr = getArray();

    if (index < 0 || static_cast<size_t>(index) >= arr->size()) {
      throw std::runtime_error("Array index out of bounds: " + key);
    }
    return (*arr)[index];
  }

  throw std::runtime_error("Cannot traverse non-container type");
}
Value& Value::get_direct_child_mut(const std::string& key) {
  if (_type == ValueTable) {
    std::map<std::string, Value>* table = getTableMut();
    std::map<std::string, Value>::iterator iter = table->find(key);

    if (iter == table->end()) {
      throw std::runtime_error("Key not found in table: " + key);
    }
    return iter->second;
  }
  if (_type == ValueArray) {
    long index = std::strtol(key.c_str(), NULL, 10);
    std::vector<Value>* arr = getArrayMut();

    if (index < 0 || static_cast<size_t>(index) >= arr->size()) {
      throw std::runtime_error("Array index out of bounds: " + key);
    }
    return (*arr)[index];
  }

  throw std::runtime_error("Cannot traverse non-container type");
}

static inline Value createValueInsert(const std::vector<PathPart>& path) {
  const PathPart& part = path.front();

  switch (part.type) {
    case PathPart::PathPartKey: {
      std::map<std::string, Value> table = std::map<std::string, Value>();

      return Value::createTable(table);
    }
    case PathPart::PathPartIndex: {
      std::vector<Value> array = std::vector<Value>();

      return Value::createArray(array);
    }
    default:
      throw std::runtime_error("Unknown path part type");
  }
}

void Value::insertOrDie(const std::vector<PathPart>& path, const Value& value) {
  if (path.empty()) {
    throw std::runtime_error("Empty path");
  }

  const PathPart& part = path.front();

  if (path.size() == 1) {
    if (part.type == PathPart::PathPartIndex) {
      std::vector<Value>* arr = getArrayMut();

      std::vector<Value>::iterator begin = arr->begin();
      std::vector<Value>::difference_type offset =
          static_cast<std::vector<Value>::difference_type>(part.index);

      if (part.index < arr->size()) {
        throw std::runtime_error("A value already exists at this index");
      }
      if (part.index > arr->size()) {
        throw std::runtime_error("The index is too big");
      }

      arr->insert(begin + offset, value);
    } else {
      std::map<std::string, Value>* tab = getTableMut();

      std::pair<std::map<std::string, Value>::iterator, bool> result =
          tab->insert(std::make_pair(part.key, value));

      if (!result.second) {
        throw std::runtime_error("A value already exists for key: " + part.key);
      }
    }
  } else {
    const std::vector<PathPart> copy(path.begin() + 1, path.end());

    if (part.type == PathPart::PathPartIndex) {
      std::vector<Value>* arr = getArrayMut();

      if (!has(part)) {
        std::vector<Value>::iterator begin = arr->begin();
        std::vector<Value>::difference_type offset =
            static_cast<std::vector<Value>::difference_type>(part.index);

        arr->insert(begin + offset, createValueInsert(copy));
      }

      arr->at(part.index).insertOrDie(copy, value);
    } else {
      std::map<std::string, Value>* tab = getTableMut();

      if (!has(part)) {
        tab->insert(std::make_pair(part.key, createValueInsert(copy)));
      }

      tab->at(part.key).insertOrDie(copy, value);
    }
  }
}

bool PathPart::operator<(const PathPart& other) const {
  if (type != other.type) {
    return type < other.type;
  }
  if (type == PathPartKey) {
    return key < other.key;
  }
  return index < other.index;
}

std::ostream& operator<<(std::ostream& ost, const Value& val) {
  switch (val.type()) {
    case ValueString:
      ost << *val.getString();
      break;
    case ValueInteger:
      ost << val.getInteger();
      break;
    case ValueFloat:
      ost << val.getFloat();
      break;
    case ValueBoolean:
      ost << (val.getBoolean() ? "true" : "false");
      break;
    case ValueArray: {
      ost << "[";
      const std::vector<Value>* arr = val.getArray();
      for (std::size_t i = 0; i < arr->size(); ++i) {
        if (i > 0) {
          ost << ", ";
        }
        ost << arr->at(i);
      }
      ost << "]";
      break;
    }
    case ValueTable: {
      ost << "{";
      const std::map<std::string, Value>* table = val.getTable();
      bool first = true;
      for (std::map<std::string, Value>::const_iterator it = table->begin();
           it != table->end(); ++it) {
        if (!first) {
          ost << ", ";
        }
        first = false;
        ost << it->first << " = " << it->second;
      }
      ost << "}";
      break;
    }
  }
  return ost;
}

}  // namespace toml98
