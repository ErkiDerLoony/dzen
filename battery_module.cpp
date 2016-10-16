#include "module.hpp"
#include "constants.hpp"
#include "battery_module.hpp"

#include <fstream>
#include <sstream>

battery_module::battery_module(const std::string filename, const int width) : module(filename), width(width) {
}

battery_module::~battery_module() {
}

void battery_module::update() {
  std::ifstream in(filename()[0]);
  in >> percent;
}

std::pair<std::string, bool> battery_module::format() const {
  std::stringstream buffer;

  buffer << "Akku ^fg(";

  if (percent < 11) {
    buffer << constants.red;
  } else if (percent < 21) {
    buffer << constants.orange;
  } else {
    buffer << constants.green;
  }

  long filled = static_cast<long>(0.5 + width * percent / 100.0);

  buffer << ")^r(" << filled << "x" << constants.height << ")^fg(" << constants.dark_grey << ")^r(" << (width - filled) << "x" << constants.height << ")^fg()";

  return std::make_pair(buffer.str(), false);
}
