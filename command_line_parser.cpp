#include "command_line_parser.hpp"

#include <iostream>

using namespace std;
using experimental::optional;

command_line_parser::command_line_parser(int argc, char** argv) {

  for (int i = 1; i < argc; i++) {
    string arg = argv[i];

    if (arg == "--") {
      continue;
    }

    if (arg.substr(0, 2) == "--") {
      // Parse long option.
      arg = arg.substr(2);
      auto index = arg.find("=");

      if (index != string::npos) {
        string key = arg.substr(0, index);
        string value = arg.substr(index + 1);
        args.insert(make_pair(key, optional<string>(value)));
      } else {

        if (i + 1 == argc
            || string(argv[i+1]).substr(0, 1) == "-"
            || string(argv[i+1]) == "--") {
          args.insert(make_pair(arg, optional<string>()));
        } else {
          args.insert(make_pair(arg, optional<string>(argv[i+1])));
          i++;
        }
      }

    } else if (arg.substr(0, 1) == "-") {
      // Parse short option.
      arg = arg.substr(1);

      if (arg.size() > 1) {
        string key = arg.substr(0, 1);
        string value = arg.substr(1);
        args.insert(make_pair(key, optional<string>(value)));
      } else {

        if (i + 1 == argc
            || string(argv[i+1]).substr(0, 1) == "-"
            || string(argv[i+1]) == "--") {
          args.insert(make_pair(arg, optional<string>()));
        } else {
          args.insert(make_pair(arg, optional<string>(argv[i+1])));
          i++;
        }
      }

    } else {
      // Parse final list of strings.

      for (int j = i; j < argc; j++) {
        list.push_back(argv[j]);
      }

      break;
    }
  }
}

command_line_parser::~command_line_parser() {
}

bool command_line_parser::has(initializer_list<string> keys) const {
  return has(keys.begin(), keys.end());
}

optional<string> command_line_parser::pop(initializer_list<string> keys) {
  return pop(keys.begin(), keys.end());
}

void command_line_parser::warn() const {

  for (auto entry : args) {
    std::cerr << "WARNING: Unused argument " << entry.first;

    if (entry.second) {
      std::cerr << " " << entry.second.value() << endl;
    } else {
      std::cerr << " (no value)" << endl;
    }
  }
}

ostream& operator<<(ostream& stream, const command_line_parser& clp) {
  stream << "{";

  for (auto entry : clp.args) {
    stream << entry.first;

    if (entry.second) {
      stream << " -> " << entry.second.value();
    }

    stream << ", ";
  }

  stream << "[";

  for (uint i = 0; i < clp.list.size(); i++) {
    stream << clp.list[i];

    if (i < clp.list.size() - 1) {
      stream << ", ";
    }
  }

  stream << "]}";
  return stream;
}
