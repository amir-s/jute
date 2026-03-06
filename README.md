<p align="center">
  <a href="https://github.com/amir-s/jute">
    <img src="jute.svg" alt="jute">
  </a>
</p>

## Very simple JSON parser for C++

A single header + implementation file JSON parser. Drop `jute.h` and `jute.cpp` into your project and go.

## Build

```bash
make        # builds the example binary
make test   # builds and runs the test suite
make clean  # removes compiled binaries
```

Or compile manually:

```bash
g++ -std=c++17 -o jute main.cpp jute.cpp
```

## Quick start

```cpp
#include "jute.h"

int main() {
    // Parse a JSON string
    jute::jValue v = jute::parser::parse(R"({"name": "jute", "version": 1})");
    std::cout << v["name"].as_string() << "\n"; // jute
    std::cout << v["version"].as_int() << "\n"; // 1

    // Or load from a file
    jute::jValue file_v = jute::parser::parse_file("data.json");
    std::cout << file_v.to_string() << "\n";
}
```

## API reference

All accessor methods are `const` — `jValue` is designed as a **read-only** view into parsed JSON.

| Method | Returns | Description |
|---|---|---|
| `get_type()` | `jType` | One of `JSTRING`, `JOBJECT`, `JARRAY`, `JBOOLEAN`, `JNUMBER`, `JNULL`, `JUNKNOWN` |
| `as_string()` | `std::string` | String value with escape sequences deserialized |
| `as_int()` | `int` | Number as integer |
| `as_double()` | `double` | Number as double |
| `as_bool()` | `bool` | Boolean value |
| `as_null()` | `void*` | Returns `nullptr` |
| `size()` | `size_t` | Number of array elements or object properties |
| `operator[](size_t i)` | `const jValue&` | Access array element by index |
| `operator[](string s)` | `const jValue&` | Access object property by key |
| `to_string()` | `std::string` | Pretty-print back to JSON |

Accessing a missing key or out-of-bounds index returns a `jValue` with type `JUNKNOWN` (no crash).

## `data.json` example

```json
{
  "examples": [
    {
      "tag_name": "a",
      "attr": [
        { "key": "href",   "value": "http://example.com" },
        { "key": "target", "value": "_blank" }
      ]
    },
    {
      "this_is": ["array", "of", "strings"],
      "number_array": [1, 2, 4, 8, 16],
      "pie": 3.14,
      "boolean": true,
      "bug": null,
      "mixed": [1, 2, {"test1": -1.2345, "test2": false}, null, 0.4, ["nested", ["array", true]], "end of story!"]
    },
    { "done": true }
  ]
}
```

## Note

This library does not perform error checking — it assumes the input is valid JSON. For production use, consider a more complete library. PRs are welcome!

License: [MIT](https://opensource.org/licenses/MIT)
