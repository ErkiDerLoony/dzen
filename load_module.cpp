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

const std::string interpolate(const std::string begin, const std::string finish, const double fraction) {

  class color {
  public:
    long red, green, blue;

    color(const std::string& c) {
      stringstream sred, sgreen, sblue;
      sred << hex << c.substr(1, 2);
      sgreen << hex << c.substr(3, 2);
      sblue << hex << c.substr(5, 2);
      sred >> red;
      sgreen >> green;
      sblue >> blue;
    }

    virtual ~color() {}

    const std::string get() const {
      stringstream buffer;
      buffer << "#" << setfill('0') << setw(2) << hex << red << setw(2) << green << setw(2) << blue;
      return buffer.str();
    }
  };

  color start(begin);
  color end(finish);

  start.red = static_cast<long>(start.red + (end.red - start.red) * fraction);
  start.green = static_cast<long>(start.green + (end.green - start.green) * fraction);
  start.blue = static_cast<long>(start.blue + (end.blue - start.blue) * fraction);

  return start.get();
}

void output(const float value, stringstream& buffer) {
  buffer << "^fg(";

  if (value < 8) {
    buffer << interpolate(constants.green, constants.yellow, value / 10.0);
  } else if (value < 12) {
    buffer << interpolate(constants.yellow, constants.orange, (value - 8.0) / 4.0);
  } else if (value < 40) {
    buffer << interpolate(constants.orange, constants.red, (value - 12.0) / (40.0 - 12.0));
  } else {
    buffer << constants.red;
  }

  buffer << ")";

  if (value < 10) {
    buffer << setprecision(2) << value;
  } else if (value < 100) {
    buffer << setprecision(1) << value;
  } else if (value < 1000) {
    buffer << setprecision(0) << value << ".";
  } else {
    buffer << setprecision(0) << value;
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
