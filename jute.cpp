#include "jute.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace jute {

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::string deserialize(const std::string &ref) {
  std::string out;
  for (size_t i = 0; i < ref.length(); i++) {
    if (ref[i] == '\\' && i + 1 < ref.length()) {
      int plus = 2;
      switch (ref[i + 1]) {
      case '"':
        out += '"';
        break;
      case '\\':
        out += '\\';
        break;
      case '/':
        out += '/';
        break;
      case 'b':
        out += '\b';
        break;
      case 'f':
        out += '\f';
        break;
      case 'n':
        out += '\n';
        break;
      case 'r':
        out += '\r';
        break;
      case 't':
        out += '\t';
        break;
      case 'u':
        if (i + 5 < ref.length()) {
          unsigned long v = 0;
          for (int j = 0; j < 4; j++) {
            v *= 16;
            char c = ref[i + 2 + j];
            if (c >= '0' && c <= '9')
              v += c - '0';
            else if (c >= 'a' && c <= 'f')
              v += c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
              v += c - 'A' + 10;
          }
          out += static_cast<char>(v);
          plus = 6;
        }
        break;
      }
      i += plus - 1;
      continue;
    }
    out += ref[i];
  }
  return out;
}

// ---------------------------------------------------------------------------
// jValue
// ---------------------------------------------------------------------------

// Static sentinel returned for missing keys / out-of-bounds indices.
static const jValue UNKNOWN_VALUE;

jValue::jValue() : type(JUNKNOWN) {}
jValue::jValue(jType tp) : type(tp) {}

std::string jValue::makesp(int d) const {
  std::string s;
  while (d--)
    s += "  ";
  return s;
}

std::string jValue::to_string_d(int d) const {
  switch (type) {
  case JSTRING:
    return std::string("\"") + svalue + "\"";
  case JNUMBER:
    return svalue;
  case JBOOLEAN:
    return svalue;
  case JNULL:
    return "null";

  case JOBJECT: {
    std::string s = "{\n";
    for (size_t i = 0; i < properties.size(); i++) {
      s += makesp(d) + "\"" + properties[i].first +
           "\": " + properties[i].second.to_string_d(d + 1) +
           (i + 1 < properties.size() ? "," : "") + "\n";
    }
    s += makesp(d - 1) + "}";
    return s;
  }

  case JARRAY: {
    std::string s = "[";
    for (size_t i = 0; i < arr.size(); i++) {
      if (i)
        s += ", ";
      s += arr[i].to_string_d(d + 1);
    }
    s += "]";
    return s;
  }

  default:
    return "##";
  }
}

std::string jValue::to_string() const { return to_string_d(1); }

jType jValue::get_type() const { return type; }

void jValue::set_type(jType tp) { type = tp; }

void jValue::add_property(const std::string &key, jValue v) {
  mpindex[key] = properties.size();
  properties.push_back(std::make_pair(key, v));
}

void jValue::add_element(jValue v) { arr.push_back(v); }

void jValue::set_string(const std::string &s) { svalue = s; }

int jValue::as_int() const {
  std::stringstream ss;
  ss << svalue;
  int k;
  ss >> k;
  return k;
}

double jValue::as_double() const {
  std::stringstream ss;
  ss << svalue;
  double k;
  ss >> k;
  return k;
}

bool jValue::as_bool() const { return svalue == "true"; }

void *jValue::as_null() const { return nullptr; }

std::string jValue::as_string() const { return deserialize(svalue); }

size_t jValue::size() const {
  switch (type) {
  case JARRAY:
    return arr.size();
  case JOBJECT:
    return properties.size();
  default:
    return 0;
  }
}

const jValue &jValue::operator[](size_t i) const {
  switch (type) {
  case JARRAY:
    if (i < arr.size())
      return arr[i];
    break;
  case JOBJECT:
    if (i < properties.size())
      return properties[i].second;
    break;
  default:
    break;
  }
  return UNKNOWN_VALUE;
}

const jValue &jValue::operator[](const std::string &s) const {
  auto it = mpindex.find(s);
  if (it == mpindex.end())
    return UNKNOWN_VALUE;
  return properties[it->second].second;
}

// ---------------------------------------------------------------------------
// parser
// ---------------------------------------------------------------------------

struct parser::token {
  std::string value;
  token_type type;
  token(std::string v = "", token_type t = UNKNOWN)
      : value(std::move(v)), type(t) {}
};

bool parser::is_whitespace(char c) {
  return isspace(static_cast<unsigned char>(c));
}

int parser::next_whitespace(const std::string &source, int i) {
  while (i < static_cast<int>(source.length())) {
    if (source[i] == '"') {
      i++;
      while (i < static_cast<int>(source.length()) &&
             (source[i] != '"' || source[i - 1] == '\\'))
        i++;
    }
    if (source[i] == '\'') {
      i++;
      while (i < static_cast<int>(source.length()) &&
             (source[i] != '\'' || source[i - 1] == '\\'))
        i++;
    }
    if (is_whitespace(source[i]))
      return i;
    i++;
  }
  return static_cast<int>(source.length());
}

int parser::skip_whitespaces(const std::string &source, int i) {
  while (i < static_cast<int>(source.length())) {
    if (!is_whitespace(source[i]))
      return i;
    i++;
  }
  return -1;
}

std::vector<parser::token> parser::tokenize(const std::string &source_in) {
  std::string source = source_in + " ";
  std::vector<token> tokens;
  int index = skip_whitespaces(source, 0);
  while (index >= 0) {
    int next = next_whitespace(source, index);
    std::string str = source.substr(index, next - index);

    size_t k = 0;
    while (k < str.length()) {
      switch (str[k]) {
      case '"': {
        size_t tmp_k = k + 1;
        while (tmp_k < str.length() &&
               (str[tmp_k] != '"' || str[tmp_k - 1] == '\\'))
          tmp_k++;
        tokens.push_back(token(str.substr(k + 1, tmp_k - k - 1), STRING));
        k = tmp_k + 1;
        continue;
      }
      case '\'': {
        size_t tmp_k = k + 1;
        while (tmp_k < str.length() &&
               (str[tmp_k] != '\'' || str[tmp_k - 1] == '\\'))
          tmp_k++;
        tokens.push_back(token(str.substr(k + 1, tmp_k - k - 1), STRING));
        k = tmp_k + 1;
        continue;
      }
      case ',':
        tokens.push_back(token(",", COMMA));
        k++;
        continue;
      case '{':
        tokens.push_back(token("{", CROUSH_OPEN));
        k++;
        continue;
      case '}':
        tokens.push_back(token("}", CROUSH_CLOSE));
        k++;
        continue;
      case '[':
        tokens.push_back(token("[", BRACKET_OPEN));
        k++;
        continue;
      case ']':
        tokens.push_back(token("]", BRACKET_CLOSE));
        k++;
        continue;
      case ':':
        tokens.push_back(token(":", COLON));
        k++;
        continue;
      case 't':
        if (k + 3 < str.length() && str.substr(k, 4) == "true") {
          tokens.push_back(token("true", BOOLEAN));
          k += 4;
          continue;
        }
        break;
      case 'f':
        if (k + 4 < str.length() && str.substr(k, 5) == "false") {
          tokens.push_back(token("false", BOOLEAN));
          k += 5;
          continue;
        }
        break;
      case 'n':
        if (k + 3 < str.length() && str.substr(k, 4) == "null") {
          tokens.push_back(token("null", NUL));
          k += 4;
          continue;
        }
        break;
      default:
        break;
      }

      if (str[k] == '-' || (str[k] >= '0' && str[k] <= '9')) {
        size_t tmp_k = k;
        if (str[tmp_k] == '-')
          tmp_k++;
        while (tmp_k < str.size() &&
               ((str[tmp_k] >= '0' && str[tmp_k] <= '9') || str[tmp_k] == '.'))
          tmp_k++;
        tokens.push_back(token(str.substr(k, tmp_k - k), NUMBER));
        k = tmp_k;
        continue;
      }

      tokens.push_back(token(str.substr(k), UNKNOWN));
      k = str.length();
    }

    index = skip_whitespaces(source, next);
  }
  return tokens;
}

jValue parser::json_parse(const std::vector<token> &v, int i, int &r) {
  jValue current;

  switch (v[i].type) {
  case CROUSH_OPEN: {
    current.set_type(JOBJECT);
    int k = i + 1;
    while (v[k].type != CROUSH_CLOSE) {
      std::string key = v[k].value;
      k += 2; // skip key and ':'
      int j = k;
      jValue vv = json_parse(v, k, j);
      current.add_property(key, vv);
      k = j;
      if (v[k].type == COMMA)
        k++;
    }
    r = k + 1;
    return current;
  }

  case BRACKET_OPEN: {
    current.set_type(JARRAY);
    int k = i + 1;
    while (v[k].type != BRACKET_CLOSE) {
      int j = k;
      jValue vv = json_parse(v, k, j);
      current.add_element(vv);
      k = j;
      if (v[k].type == COMMA)
        k++;
    }
    r = k + 1;
    return current;
  }

  case NUMBER:
    current.set_type(JNUMBER);
    current.set_string(v[i].value);
    r = i + 1;
    return current;

  case STRING:
    current.set_type(JSTRING);
    current.set_string(v[i].value);
    r = i + 1;
    return current;

  case BOOLEAN:
    current.set_type(JBOOLEAN);
    current.set_string(v[i].value);
    r = i + 1;
    return current;

  case NUL:
    current.set_type(JNULL);
    current.set_string("null");
    r = i + 1;
    return current;

  default:
    return current;
  }
}

jValue parser::parse(const std::string &str) {
  int k;
  return json_parse(tokenize(str), 0, k);
}

jValue parser::parse_file(const std::string &filename) {
  std::ifstream in(filename);
  std::string str, tmp;
  while (std::getline(in, tmp))
    str += tmp;
  in.close();
  return parse(str);
}

} // namespace jute