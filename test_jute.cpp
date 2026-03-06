// test_jute.cpp — lightweight self-contained tests for jute
// No external framework required; failures print to stderr and exit non-zero.

#include "jute.h"
#include <cassert>
#include <iostream>
#include <string>

static int passed = 0;
static int failed = 0;

#define CHECK(expr, label)                                                     \
  do {                                                                         \
    if (expr) {                                                                \
      std::cout << "  PASS: " << label << "\n";                                \
      ++passed;                                                                \
    } else {                                                                   \
      std::cerr << "  FAIL: " << label << "\n";                                \
      ++failed;                                                                \
    }                                                                          \
  } while (0)

// ---------------------------------------------------------------------------

void test_string() {
  std::cout << "[string]\n";
  auto v = jute::parser::parse(R"({"name": "jute"})");
  CHECK(v["name"].get_type() == jute::JSTRING, "type is JSTRING");
  CHECK(v["name"].as_string() == "jute", "value is 'jute'");
}

void test_number() {
  std::cout << "[number]\n";
  auto v = jute::parser::parse(R"({"pi": 3.14, "n": -7})");
  CHECK(v["pi"].get_type() == jute::JNUMBER, "pi type is JNUMBER");
  CHECK(v["pi"].as_double() == 3.14, "pi value is 3.14");
  CHECK(v["n"].as_int() == -7, "n value is -7");
}

void test_boolean() {
  std::cout << "[boolean]\n";
  auto v = jute::parser::parse(R"({"yes": true, "no": false})");
  CHECK(v["yes"].get_type() == jute::JBOOLEAN, "yes type is JBOOLEAN");
  CHECK(v["yes"].as_bool() == true, "yes is true");
  CHECK(v["no"].as_bool() == false, "no is false");
}

void test_null() {
  std::cout << "[null]\n";
  auto v = jute::parser::parse(R"({"nothing": null})");
  CHECK(v["nothing"].get_type() == jute::JNULL, "type is JNULL");
  CHECK(v["nothing"].as_null() == nullptr, "as_null() == nullptr");
}

void test_array() {
  std::cout << "[array]\n";
  auto v = jute::parser::parse(R"([1, 2, 3])");
  CHECK(v.get_type() == jute::JARRAY, "type is JARRAY");
  CHECK(v.size() == 3, "size is 3");
  CHECK(v[0].as_int() == 1, "[0] == 1");
  CHECK(v[1].as_int() == 2, "[1] == 2");
  CHECK(v[2].as_int() == 3, "[2] == 3");
}

void test_nested() {
  std::cout << "[nested]\n";
  auto v = jute::parser::parse(R"({"a": {"b": {"c": 42}}})");
  CHECK(v["a"]["b"]["c"].as_int() == 42, "a.b.c == 42");
}

void test_array_of_objects() {
  std::cout << "[array of objects]\n";
  auto v = jute::parser::parse(R"([{"x": 1}, {"x": 2}])");
  CHECK(v.size() == 2, "size is 2");
  CHECK(v[0]["x"].as_int() == 1, "[0].x == 1");
  CHECK(v[1]["x"].as_int() == 2, "[1].x == 2");
}

void test_escape_sequences() {
  std::cout << "[escape sequences]\n";
  // JSON: {"msg": "hello\nworld"} — the \n should deserialize to a real newline
  // We build the string programmatically to avoid raw-string escaping
  // confusion.
  std::string json = "{\"msg\": \"hello\\nworld\"}";
  auto v = jute::parser::parse(json);
  std::string s = v["msg"].as_string();
  CHECK(s.find('\n') != std::string::npos,
        "contains newline after deserialize");
}

void test_object_size() {
  std::cout << "[object size]\n";
  auto v = jute::parser::parse(R"({"a": 1, "b": 2, "c": 3})");
  CHECK(v.get_type() == jute::JOBJECT, "type is JOBJECT");
  CHECK(v.size() == 3, "size is 3");
}

void test_missing_key_is_unknown() {
  std::cout << "[missing key returns JUNKNOWN]\n";
  auto v = jute::parser::parse(R"({"x": 1})");
  CHECK(v["missing"].get_type() == jute::JUNKNOWN, "missing key is JUNKNOWN");
}

void test_to_string_roundtrip() {
  std::cout << "[to_string]\n";
  auto v = jute::parser::parse(R"({"flag": true})");
  // to_string should contain the key and value somewhere
  std::string s = v.to_string();
  CHECK(s.find("flag") != std::string::npos, "to_string contains 'flag'");
  CHECK(s.find("true") != std::string::npos, "to_string contains 'true'");
}

// ---------------------------------------------------------------------------

int main() {
  std::cout << "=== jute tests ===\n";
  test_string();
  test_number();
  test_boolean();
  test_null();
  test_array();
  test_nested();
  test_array_of_objects();
  test_escape_sequences();
  test_object_size();
  test_missing_key_is_unknown();
  test_to_string_roundtrip();
  std::cout << "\n" << passed << " passed, " << failed << " failed.\n";
  return failed == 0 ? 0 : 1;
}
