#include "who_module.hpp"

#include <fstream>
#include <sstream>

using namespace std;

map<string, uint> who_module::lengths;

who_module::who_module(const string filename) : module(filename) {}

who_module::~who_module() {}

void who_module::update() {
  ifstream in(filename());
  string line;
  names.clear();

  while (getline(in, line)) {
    stringstream s(line);
    string name;
    s >> name;
    names.insert(name);
  }
}

string who_module::format() const {
  uint count = 0;
  stringstream s;

  for (const string name : names) {
    s << name;

    if (count < names.size() - 1) {
      s << ", ";
    }

    count++;
  }

  const uint pre_len = s.str().length();
  lengths[filename()] = pre_len;
  const uint max_len = max_length();

  if (pre_len < max_len) {

    for (uint i = 0; i < max_len - pre_len; i++) {
      s << " ";
    }
  }

  return s.str();
}

const uint who_module::max_length() const {
  uint result = 0;

  for (pair<string, uint> entry : lengths) {
    result += entry.second;
  }

  return result;
}
