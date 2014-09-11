#include "modules.hpp"
#include "cpu_module.hpp"

#include <sstream>

using namespace std;

local_modules::local_modules(const int width) : cpu("/proc/stat", false, true, static_cast<int>(0.75*width)),
                                                mem("/proc/meminfo", width) {
}

void local_modules::update() {
  cpu.update();
  mem.update();
}

string local_modules::format() const {
  stringstream buffer;
  buffer << cpu.format();
  buffer << "   ";
  buffer << mem.format();
  return buffer.str();
}

remote_modules::remote_modules(const string hostname, const int width) : host(hostname),
                                                                         cpu(remote_wrapper("/proc/stat", hostname, unique_ptr<module>(new cpu_module("/tmp/" + hostname + ".stat", true, false, 2*width)))),
                                                                         mem(remote_wrapper("/proc/meminfo", hostname, unique_ptr<module>(new mem_module("/tmp/" + hostname + ".meminfo", 2*width)))) {
}

remote_modules::remote_modules(remote_modules&& other) : host(move(other.host)),
                                                         cpu(move(other.cpu)),
                                                         mem(move(other.mem)) {
}

void remote_modules::update() {
  cpu.update();
  mem.update();
}

string remote_modules::format() const {
  stringstream buffer;
  buffer << cpu.format();
  buffer << "   ";
  buffer << mem.format();
  return buffer.str();
}

const string remote_modules::hostname() const {
  return host;
}
