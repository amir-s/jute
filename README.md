Jute :: Very simple JSON parser for c++
---------
	
`data.json`:

	{
		"examples": [
			{
				"tag_name": "a",
				"attr": [{"key": "href", "value": "http://amir.saboury.net"}, {"key": "target", "value": "_blank"}]
			},
			{
				"this_is": ["array", "of", "strings"],
				"number_array": [1, 2, 4, 8, 16],
				"pie": 3.14,
				"boolean": true,
				"bug": null,
				"mixed": [1, 2, {"test1": -1.2345, "test2": false}, null, 0.4, ["nested", ["array", true]], "end of story!"]
			},
			{
				"done": true
			}
		]
	}

example `main.cpp` file:

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
		cout << " ------ " << endl;
		cout << v["examples"][0]["attr"][0]["value"].as_string() << endl;
		if (v["examples"][1]["mixed"][5][1][1].as_bool()) {
			cout << v["examples"][1]["pie"].as_double() << endl;
			cout << v["examples"][2].to_string() << endl;
		}
		// You can get type of a jValue by calling its get_type() function
		// It returns a jType which can be one of these: 
		//    {JSTRING, JOBJECT, JARRAY, JBOOLEAN, JNUMBER, JNULL, JUNKNOWN}
		//
		//    if (v["examples"][1]["mixed"][5][1][1].get_type() == jute::JBOOLEAN) ...
		return 0;
	}

Note
----

This version is not providing error checking. All functions assume the input string is in valid format of JSON. Also number format checking is poor.

Improvments are welcome. Read the source code, it is just about 300 LOC. :)

