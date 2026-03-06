#ifndef __JUTE_H__
#define __JUTE_H__

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>

namespace jute {
  enum jType { JSTRING, JOBJECT, JARRAY, JBOOLEAN, JNUMBER, JNULL, JUNKNOWN };

  class jValue {
    friend class parser;

  private:
    std::string makesp(int d) const;
    std::string svalue;
    jType type;
    std::vector<std::pair<std::string, jValue>> properties;
    std::map<std::string, size_t> mpindex;
    std::vector<jValue> arr;
    std::string to_string_d(int d) const;

    // Write API — only accessible by the parser
    void set_type(jType tp);
    void add_property(const std::string& key, jValue v);
    void add_element(jValue v);
    void set_string(const std::string& s);

  public:
    jValue();
    explicit jValue(jType tp);

    std::string to_string() const;
    jType get_type() const;

    int as_int() const;
    double as_double() const;
    bool as_bool() const;
    void* as_null() const;
    std::string as_string() const;

    size_t size() const;

    const jValue& operator[](size_t i) const;
    const jValue& operator[](const std::string& s) const;
  };

  class parser {
  private:
    enum token_type {
      UNKNOWN, STRING, NUMBER,
      CROUSH_OPEN, CROUSH_CLOSE,
      BRACKET_OPEN, BRACKET_CLOSE,
      COMMA, COLON, BOOLEAN, NUL
    };

    struct token;
    static bool is_whitespace(char c);
    static int next_whitespace(const std::string& source, int i);
    static int skip_whitespaces(const std::string& source, int i);
    static std::vector<token> tokenize(const std::string& source);
    static jValue json_parse(const std::vector<token>& v, int i, int& r);

  public:
    static jValue parse(const std::string& str);
    static jValue parse_file(const std::string& filename);
  };
}

#endif
