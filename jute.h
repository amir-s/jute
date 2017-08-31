#ifndef __JUTE_H__
#define __JUTE_H__

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
using namespace std;
namespace jute {
  enum jType {JSTRING, JOBJECT, JARRAY, JBOOLEAN, JNUMBER, JNULL, JUNKNOWN};
  class jValue {
    private:
      string makesp(int);
      string svalue;
      jType type;
      vector<pair<string, jValue> > properties;
      map<string, int> mpindex;
      vector<jValue> arr;
      string to_string_d(int);
    public:
      jValue();
      jValue(jType);
      string to_string();
      jType get_type();
      void set_type(jType);
      void add_property(string key, jValue v);
      void add_element(jValue v);
      void set_string(string s);
      int as_int();
      double as_double();
      bool as_bool();
      void* as_null();
      string as_string();
      int size();
      jValue operator[](int i);
      jValue operator[](string s);
  };

  class parser {
    private:
    enum token_type {UNKNOWN, STRING, NUMBER, CROUSH_OPEN, CROUSH_CLOSE, BRACKET_OPEN, BRACKET_CLOSE, COMMA, COLON, BOOLEAN, NUL};

    struct token;
    static bool is_whitespace(const char c);
    static int next_whitespace(const string& source, int i);
    static int skip_whitespaces(const string& source, int i);

    static vector<token> tokenize(string source);
    static jValue json_parse(vector<token> v, int i, int& r);
    public: 
    static jValue parse(const string& str);
    static jValue parse_file(const string& str);
  };
}
#endif
