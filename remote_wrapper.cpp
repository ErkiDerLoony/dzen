#include "remote_wrapper.hpp"

#include <iostream>

using namespace std;

remote_wrapper::remote_wrapper(const string remote_command, const string hostname, unique_ptr<module> inner)
  : module(inner->filename()),
    ok(false),
    remote_command(remote_command),
    hostname(hostname),
    inner(move(inner)) {
}

remote_wrapper::remote_wrapper(remote_wrapper&& other)
  : module(other.inner->filename()),
    ok(false),
    remote_command(other.remote_command),
    hostname(other.hostname) {
  inner = move(other.inner);
}

remote_wrapper::~remote_wrapper() {
}

void remote_wrapper::update() {

  for (string filename : inner->filename()) {
    const int result = system(string("ssh -o ConnectTimeout=5 " + hostname + " '" + remote_command + "' > " + filename + " 2>/dev/null").c_str());

    if (result == 0) {
      ok = true;
      inner->update();
      remove(filename.c_str());
    } else {
      ok = false;
      break;
    }
  }
}

pair<string, bool> remote_wrapper::format() const {

  if (ok) {
    return inner->format();
  } else {
    return make_pair("<unreachable>", false);
  }
}
