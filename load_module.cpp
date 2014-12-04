#include "load_module.hpp"
#include "constants.hpp"

#include <ios>
#include <iomanip>
#include <sstream>
#include <fstream>

using namespace std;

load_module::load_module(const string filename) : module(filename) {}

load_module::~load_module() {}

void load_module::update() {
  ifstream in(filename());
  string line;
  getline(in, line);
  stringstream s(line);
  s >> avg1 >> avg5 >> avg15;
}

void output(const float value, stringstream& buffer) {

  if (value < 10) {
    buffer << setprecision(2) << value;
  } else if (value < 100) {
    buffer << "^fg(" << constants.yellow << ")" << setprecision(1) << value;
  } else if (value < 1000) {
    buffer << "^fg(" << constants.orange << ")" << setprecision(0) << value << ".";
  } else {
    buffer << "^fg(" << constants.red << ")" << setprecision(0) << value;
  }

  buffer << "^fg()";
}

string load_module::format() const {
  stringstream buffer;
  buffer << fixed;
  output(avg1, buffer);
  buffer << " ";
  output(avg5, buffer);
  buffer << " ";
  output(avg15, buffer);
  return buffer.str();
}
