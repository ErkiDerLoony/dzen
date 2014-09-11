#include "module.hpp"
#include "constants.hpp"

using std::string;

module::module(const string filename) : _filename(filename) {
}

module::~module() {
}

const string& module::filename() const {
  return _filename;
}

void module::print(std::ostream& stream, const long& value, const string& colour) const {
  stream << "^fg(" << colour << ")^r(" << value << "x" << constants.height << ")";
}
