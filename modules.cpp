#include "modules.hpp"
#include "cpu_module.hpp"

#include <sstream>

using namespace std;

local_modules::local_modules(const int width) : cpu("/proc/stat", false, true, static_cast<int>(0.75*width)) {
}

void local_modules::update() {
  cpu.update();
}

string local_modules::format() const {
  stringstream buffer;
  buffer << cpu.format();
  return buffer.str();
}

remote_modules::remote_modules(const string hostname, const int width) : host(hostname), cpu(remote_wrapper(hostname, unique_ptr<module>(new cpu_module("/tmp/" + hostname + ".stat", true, false, 2*width)))) {
}

remote_modules::remote_modules(remote_modules&& other) : host(move(other.host)), cpu(move(other.cpu)) {
}

void remote_modules::update() {
  cpu.update();
}

string remote_modules::format() const {
  stringstream buffer;
  buffer << cpu.format();
  return buffer.str();
}

const string remote_modules::hostname() const {
  return host;
}
