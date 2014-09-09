#include "module.hpp"

using std::string;

module::module(const string filename) : _filename(filename) {
}

module::~module() {
}

const string& module::filename() const {
  return _filename;
}
