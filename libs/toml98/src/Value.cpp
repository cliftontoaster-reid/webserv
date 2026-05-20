#include "Value.hpp"

#include <cstddef>
#include <cstring>
#include <stdexcept>

namespace toml98 {

Value Value::createString(const std::string& val) {
  std::string* ptr = new std::string(val);
  return Value(ValueString, reinterpret_cast<void*>(ptr));
}

Value Value::createInteger(uint64_t val) { return Value(ValueInteger, val); }

Value Value::createFloat(double val) {
  uint64_t bits;
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
  return reinterpret_cast<const std::string*>(_ptr);
}

uint64_t Value::getInteger() const {
  if (_type != ValueInteger) {
    throw std::runtime_error("Value is not an integer.");
  }
  return _nbr;
}

double Value::getFloat() const {
  if (_type != ValueFloat) {
    throw std::runtime_error("Value is not a float.");
  }
  uint64_t bits = _nbr;
  double val;
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
  return reinterpret_cast<const std::vector<Value>*>(_ptr);
}

const std::map<std::string, Value>* Value::getTable() const {
  if (_type != ValueTable) {
    throw std::runtime_error("Value is not a table.");
  }
  return reinterpret_cast<const std::map<std::string, Value>*>(_ptr);
}

Value::Value(const Value& other) : _type(other._type), _ptr(NULL) {
  switch (_type) {
    case ValueString: {
      const std::string* src = reinterpret_cast<const std::string*>(other._ptr);
      _ptr = reinterpret_cast<void*>(new std::string(*src));
      break;
    }
    case ValueArray: {
      const std::vector<Value>* src =
          reinterpret_cast<const std::vector<Value>*>(other._ptr);
      _ptr = reinterpret_cast<void*>(new std::vector<Value>(*src));
      break;
    }
    case ValueTable: {
      const std::map<std::string, Value>* src =
          reinterpret_cast<const std::map<std::string, Value>*>(other._ptr);
      _ptr = reinterpret_cast<void*>(new std::map<std::string, Value>(*src));
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
        const std::string* src =
            reinterpret_cast<const std::string*>(other._ptr);
        _ptr = reinterpret_cast<void*>(new std::string(*src));
        break;
      }
      case ValueArray: {
        const std::vector<Value>* src =
            reinterpret_cast<const std::vector<Value>*>(other._ptr);
        _ptr = reinterpret_cast<void*>(new std::vector<Value>(*src));
        break;
      }
      case ValueTable: {
        const std::map<std::string, Value>* src =
            reinterpret_cast<const std::map<std::string, Value>*>(other._ptr);
        _ptr = reinterpret_cast<void*>(new std::map<std::string, Value>(*src));
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
        std::string* str = reinterpret_cast<std::string*>(oldPtr);
        delete str;
        break;
      }
      case ValueArray: {
        std::vector<Value>* array =
            reinterpret_cast<std::vector<Value>*>(oldPtr);
        delete array;
        break;
      }
      case ValueTable: {
        std::map<std::string, Value>* table =
            reinterpret_cast<std::map<std::string, Value>*>(oldPtr);
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
      std::string* str = reinterpret_cast<std::string*>(_ptr);
      delete str;
      break;
    }
    case ValueArray: {
      std::vector<Value>* array = reinterpret_cast<std::vector<Value>*>(_ptr);
      delete array;
      break;
    }
    case ValueTable: {
      std::map<std::string, Value>* table =
          reinterpret_cast<std::map<std::string, Value>*>(_ptr);
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
  if (_type != other._type) return false;
  switch (_type) {
    case ValueString:
      return *reinterpret_cast<const std::string*>(_ptr) ==
             *reinterpret_cast<const std::string*>(other._ptr);
    case ValueInteger:
      return _nbr == other._nbr;
    case ValueFloat: {
      double a, b;
      std::memcpy(&a, &_nbr, sizeof(a));
      std::memcpy(&b, &other._nbr, sizeof(b));
      return a == b;
    }
    case ValueBoolean:
      return static_cast<bool>(_nbr) == static_cast<bool>(other._nbr);
    case ValueArray:
      return *reinterpret_cast<const std::vector<Value>*>(_ptr) ==
             *reinterpret_cast<const std::vector<Value>*>(other._ptr);
    case ValueTable:
      return *reinterpret_cast<const std::map<std::string, Value>*>(_ptr) ==
             *reinterpret_cast<const std::map<std::string, Value>*>(other._ptr);
  }
  return false;
}

bool Value::operator!=(const Value& other) const { return !(*this == other); }

Value::Value(ValueType type, void* val) : _type(type), _ptr(val) {}
Value::Value(ValueType type, uint64_t val) : _type(type), _nbr(val) {}
Value::Value() : _type(ValueString) {}

std::ostream& operator<<(std::ostream& os, const Value& val) {
  switch (val.type()) {
    case ValueString:
      os << *val.getString();
      break;
    case ValueInteger:
      os << val.getInteger();
      break;
    case ValueFloat:
      os << val.getFloat();
      break;
    case ValueBoolean:
      os << (val.getBoolean() ? "true" : "false");
      break;
    case ValueArray: {
      os << "[";
      const std::vector<Value>* arr = val.getArray();
      for (std::size_t i = 0; i < arr->size(); ++i) {
        if (i > 0) os << ", ";
        os << (*arr)[i];
      }
      os << "]";
      break;
    }
    case ValueTable: {
      os << "{";
      const std::map<std::string, Value>* table = val.getTable();
      bool first = true;
      for (std::map<std::string, Value>::const_iterator it = table->begin();
           it != table->end(); ++it) {
        if (!first) os << ", ";
        first = false;
        os << it->first << " = " << it->second;
      }
      os << "}";
      break;
    }
  }
  return os;
}

}  // namespace toml98
