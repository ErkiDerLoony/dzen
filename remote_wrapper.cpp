#include "remote_wrapper.hpp"

#include <iostream>

using namespace std;

remote_wrapper::remote_wrapper(const vector<string> remote_commands, const string hostname, unique_ptr<module> inner)
  : module(inner->filename()),
    ok(false),
    remote_commands(remote_commands),
    hostname(hostname),
    inner(move(inner)) {
}

remote_wrapper::remote_wrapper(remote_wrapper&& other)
  : module(other.inner->filename()),
    ok(false),
    remote_commands(other.remote_commands),
    hostname(other.hostname) {
  inner = move(other.inner);
}

remote_wrapper::~remote_wrapper() {
}

void remote_wrapper::update() {
  uint index = 0;

  for (string filename : inner->filename()) {
    const int result = system(string("ssh -o ConnectTimeout=5 " + hostname + " '" + remote_commands[index++] + "' > " + filename + " 2>/dev/null").c_str());

    if (result == 0) {
      ok = true;
    } else {
      ok = false;
      break;
    }
  }

  if (ok) {
    inner->update();
  }

  for (string filename : inner->filename()) {
    remove(filename.c_str());
  }
}

pair<string, bool> remote_wrapper::format() const {

  if (ok) {
    return inner->format();
  } else {
    return make_pair("<unreachable>", false);
  }
}
