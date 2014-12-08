#include "module.hpp"
#include "constants.hpp"

using namespace std;

module::module(const string filename) : module({filename}) {
}

module::module(const initializer_list<string> filenames) : files(filenames) {
}

module::module(const vector<string> filenames) : files(filenames) {
}

module::~module() {
}

vector<string> module::filename() const {
  return files;
}

void module::print(std::ostream& stream, const long& value, const string& colour) const {
  stream << "^fg(" << colour << ")^r(" << value << "x" << constants.height << ")";
}
