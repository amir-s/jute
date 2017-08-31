#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#include "jute.h"
using namespace std;

int main () {
  
  ifstream in("data.json");
  string str = "";
  string tmp;
  while (getline(in, tmp)) str += tmp;
  jute::jValue v = jute::parser::parse(str);
  cout << v.to_string() << endl;

  // you can use jute::parser::parse_file("data.json")

  cout << " ------ " << endl;
  cout << v["examples"][0]["attr"][0]["value"].as_string() << endl;
  if (v["examples"][1]["mixed"][5][1][1].as_bool()) {
    cout << v["examples"][1]["pie"].as_double() << endl;
    cout << v["examples"][2].to_string() << endl;
  }
  // You can get type of a jValue by calling get_type() function
  // It returns a jType which can be one of these: 
  //    {JSTRING, JOBJECT, JARRAY, JBOOLEAN, JNUMBER, JNULL, JUNKNOWN}
  //
  //    if (v["examples"][1]["mixed"][5][1][1].get_type() == jute::JBOOLEAN) ...
  // 
  // if you use 'as_string' function, all control characters will be deserialized
  // cout << v["examples"][3]["control_chars"].as_string() << endl;
  return 0;
}

