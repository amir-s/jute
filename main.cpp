#include "jute.h"
#include <iostream>

int main() {
  // Parse from file (or use jute::parser::parse for inline strings)
  jute::jValue v = jute::parser::parse_file("data.json");
  std::cout << v.to_string() << "\n";

  std::cout << " ------ \n";
  std::cout << v["examples"][0]["attr"][0]["value"].as_string() << "\n";
  if (v["examples"][1]["mixed"][5][1][1].as_bool()) {
    std::cout << v["examples"][1]["pie"].as_double() << "\n";
    std::cout << v["examples"][2].to_string() << "\n";
  }

  // Check type before accessing
  if (v["examples"][1]["mixed"][5][1][1].get_type() == jute::JBOOLEAN) {
    std::cout << "(confirmed boolean)\n";
  }

  // control chars: as_string() deserializes escape sequences
  // std::cout << v["examples"][3]["control_chars"].as_string() << "\n";

  return 0;
}
