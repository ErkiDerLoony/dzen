#include "load_module.hpp"
#include "constants.hpp"

#include <ios>
#include <iomanip>
#include <sstream>
#include <fstream>

using namespace std;

load_module::load_module(const string loadavg, const string cpuinfo) : module({loadavg, cpuinfo}) {}

load_module::~load_module() {}

void load_module::update() {
  ifstream in(filename()[0]);
  string line;
  getline(in, line);
  stringstream s(line);
  s >> avg1 >> avg5 >> avg15;

  ifstream in2(filename()[1]);
  nproc = 0;

  while (getline(in2, line)) {

    if (line.substr(0, 9) == "processor") {
      nproc++;
    }
  }
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

void output(const float value, const uint nproc, stringstream& buffer) {
  buffer << "^fg(";

  const float limit0 = nproc/2.0;
  const float limit1 = nproc;
  const float limit2 = 1.5*nproc;

  if (value < limit0) {
    buffer << interpolate(constants.green, constants.yellow, value / limit0);
  } else if (value < limit1) {
    buffer << interpolate(constants.yellow, constants.orange, (value - limit0) / (limit1 - limit0));
  } else if (value < limit2) {
    buffer << interpolate(constants.orange, constants.bright_red, (value - limit1) / (limit2 - limit1));
  } else {
    buffer << constants.bright_red;
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

pair<string, bool> load_module::format() const {
  stringstream buffer;
  buffer << fixed;
  output(avg1, nproc, buffer);
  buffer << " ";
  output(avg5, nproc, buffer);
  buffer << " ";
  output(avg15, nproc, buffer);
  return make_pair(buffer.str(), false);
}
