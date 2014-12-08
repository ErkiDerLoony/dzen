#include "uptime_module.hpp"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <ios>

using namespace std;

uptime_module::uptime_module(const string filename, const uint padding) : module(filename), days(0), padding(padding) {}

uptime_module::~uptime_module() {}

void uptime_module::update() {
  ifstream in(filename()[0]);
  uint seconds;
  in >> seconds;
  days = seconds / 60 / 60 / 24;
}

pair<string, bool> uptime_module::format() const {
  stringstream buffer;
  buffer << "up " << setw(padding) << setfill(' ') << days << " day" << (days == 1 ? "" : "s");
  return make_pair(buffer.str(), false);
}
