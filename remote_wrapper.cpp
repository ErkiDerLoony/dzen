#include "remote_wrapper.hpp"

#include <iostream>

using namespace std;

remote_wrapper::remote_wrapper(const string remote_filename, const string hostname, unique_ptr<module> inner)
  : module(inner->filename()),
    ok(false),
    remote_filename(remote_filename),
    hostname(hostname),
    inner(move(inner)) {
}

remote_wrapper::remote_wrapper(remote_wrapper&& other)
  : module(other.inner->filename()),
    ok(false),
    remote_filename(other.remote_filename),
    hostname(other.hostname) {
  inner = move(other.inner);
}

remote_wrapper::~remote_wrapper() {
}

void remote_wrapper::update() {
  const int result = system(string("ssh " + hostname + " 'cat " + remote_filename + "' > " + inner->filename() + " 2>/dev/null").c_str());

  if (result == 0) {
    ok = true;
    inner->update();
    remove(filename().c_str());
  } else {
    ok = false;
  }
}

string remote_wrapper::format() const {

  if (ok) {
    return inner->format();
  } else {
    return "<unreachable>";
  }
}
