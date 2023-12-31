#ifndef PARSER_HPP
#define PARSER_HPP
#include "tokenizer.hpp"
#include <iostream>
#include <memory>
#include <unordered_map>
#include <variant>

namespace JsonDerulo {

#define sPtr std::shared_ptr

enum class ValueType {
  STRING,
  DOUBLE,
  INT,
  OBJECT,
  BOOL,
  ARRAY,
};

struct Value;

struct Object {
private:
  int depth = 0;

public:
  std::unordered_map<std::string, Value> elements;
  std::string to_str() const;

  Object() {}
  Object(int depth) : depth(depth) {}
  friend std::ostream &operator<<(std::ostream &os, const Object &jo);

  template <typename T> inline void insert(const std::string key, T val);

  /*
  indexes into the object with the given key
  */
  Value &operator[](const std::string key);

  /*
  gets the value at the given key. If the value is not of the given type,
  throws an exception
  */
  template <typename T> sPtr<T> get(const std::string &key) const;
  /*
  gets the value at the given key. If the value is not of the given type,
  returns the default value
  */
  template <typename T>
  sPtr<T> get(const std::string &key, const T default_val) const;
  /*
  gets the value at the given key. If the value is not of the given type,
  returns nullptr
  */
  template <typename T> sPtr<T> try_get(const std::string &key) const;
};

struct Array {
private:
  int depth;

public:
  std::vector<Value> elements;
  std::string to_str() const;
  Array(int depth) : depth(depth) {}
  /*
  returns the value at the given index
  */
  Value &operator[](int idx) { return elements[idx]; }
  friend std::ostream &operator<<(std::ostream &os, const Array &ja);
};

/*
the possible types of values in a JSON object
*/
using val_t = std::variant<sPtr<int>, sPtr<double>, sPtr<bool>,
                           sPtr<std::string>, sPtr<Object>, sPtr<Array>>;

struct Value {
  ValueType type;
  val_t value;
  Value() {}
  Value(int value)
      : type(ValueType::INT), value(std::make_shared<int>(value)) {}
  Value(double value)
      : type(ValueType::DOUBLE), value(std::make_shared<double>(value)) {}
  Value(bool value)
      : type(ValueType::BOOL), value(std::make_shared<bool>(value)) {}
  Value(std::string value)
      : type(ValueType::STRING), value(std::make_shared<std::string>(value)) {}
  Value(Object value)
      : type(ValueType::OBJECT), value(std::make_shared<Object>(value)) {}
  Value(Array value)
      : type(ValueType::ARRAY), value(std::make_shared<Array>(value)) {}
  Value(ValueType t, val_t value) : type(t), value(value) {}
  std::string to_str() const;
  /*
  casts the value to the given type. If the value is not of the given type,
  throws an exception
  */
  template <typename T> sPtr<T> cast() const {
    return std::get<sPtr<T>>(value);
  }

  /*
  attempts to cast the value to the given type. If the value is not of the given
  type, returns nullptr
  */
  template <typename T> sPtr<T> try_cast() const {
    if (std::get_if<sPtr<T>>(value)) {
      return std::get<sPtr<T>>(value);
    } else {
      return nullptr;
    }
  }

  Value &operator[](const std::string key) const {
    return std::get<sPtr<Object>>(value)->operator[](key);
  }

  friend std::ostream &operator<<(std::ostream &os, const Value &jv);
};

struct JSON {

  Object root;

  /*
   * Returns a string representation of the JSON object
   */
  std::string to_str() const { return root.to_str(); }

  friend std::ostream &operator<<(std::ostream &os, const JSON &json);
  Value &operator[](const std::string key) { return root[key]; }
};

class Parser {
private:
  Tokenizer &tokenizer;
  int idx = 0;
  int depth = 0;
  JSON object;
  Object expect_object();
  Value expect_value();
  Array expect_array();

public:
  Parser(Tokenizer &tokenizer) : tokenizer(tokenizer) {}
  JSON parse();
};

#include "parser.tpp"
#endif

} // namespace json_reader
