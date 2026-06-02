#include "Value.hpp"

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

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
    throw std::runtime_error("Value is not a string.");
  }
  return static_cast<const std::string*>(_ptr);
}

int64_t Value::getInteger() const {
  if (_type != ValueInteger) {
    throw std::runtime_error("Value is not an integer.");
  }
  return static_cast<int64_t>(_nbr);
}

double Value::getFloat() const {
  if (_type != ValueFloat) {
    throw std::runtime_error("Value is not a float.");
  }
  uint64_t bits = _nbr;
  double val = NAN;
  std::memcpy(&val, &bits, sizeof(val));
  return val;
}

bool Value::getBoolean() const {
  if (_type != ValueBoolean) {
    throw std::runtime_error("Value is not a boolean.");
  }
  return static_cast<bool>(_nbr);
}

const std::vector<Value>* Value::getArray() const {
  if (_type != ValueArray) {
    throw std::runtime_error("Value is not an array.");
  }
  return static_cast<const std::vector<Value>*>(_ptr);
}

const std::map<std::string, Value>* Value::getTable() const {
  if (_type != ValueTable) {
    throw std::runtime_error("Value is not a table.");
  }
  return static_cast<const std::map<std::string, Value>*>(_ptr);
}

std::vector<Value>* Value::getArrayMut() {
  if (_type != ValueArray) {
    throw std::runtime_error("Value is not an array.");
  }
  return static_cast<std::vector<Value>*>(_ptr);
}

std::map<std::string, Value>* Value::getTableMut() {
  if (_type != ValueTable) {
    throw std::runtime_error("Value is not a table.");
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

const Value& Value::get(const std::string& path) const {
  std::string::size_type dot = path.find('.');

  if (dot == std::string::npos) {
    return get_direct_child(path);
  }

  std::string key = path.substr(0, dot);
  std::string remaining = path.substr(dot + 1);

  return get_direct_child(key).get(remaining);
}

Value& Value::get_mut(const std::string& path) {
  std::string::size_type dot = path.find('.');

  if (dot == std::string::npos) {
    return get_direct_child_mut(path);
  }

  std::string key = path.substr(0, dot);
  std::string remaining = path.substr(dot + 1);

  return get_direct_child_mut(key).get_mut(remaining);
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
