#include "remote_wrapper.hpp"

#include <iostream>

using namespace std;

remote_wrapper::remote_wrapper(const string hostname, unique_ptr<module> inner) : module(inner->filename()), hostname(hostname), inner(move(inner)) {
}

remote_wrapper::remote_wrapper(remote_wrapper&& other) : module(other.inner->filename()), hostname(other.hostname) {
  inner = move(other.inner);
}

remote_wrapper::~remote_wrapper() {
}

void remote_wrapper::update() {
  const int result = system(string("ssh " + hostname + " 'cat /proc/stat' > " + inner->filename() + " 2>/dev/null").c_str());

  if (result == 0) {
    inner->update();
    remove(filename().c_str());
  }
}

string remote_wrapper::format() const {
  return inner->format();
}
