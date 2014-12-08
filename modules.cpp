#include "modules.hpp"

#include <sstream>

using namespace std;

local_modules::local_modules(const int width) : cpu("/proc/stat", false, true, static_cast<int>(0.75*width)),
                                                mem("/proc/meminfo", width),
                                                load("/proc/loadavg"),
                                                net("/proc/net/dev"),
                                                uptime("/proc/uptime", 1) {
}

void local_modules::update() {
  cpu.update();
  mem.update();
  load.update();
  net.update();
  uptime.update();
}

string local_modules::format() const {
  stringstream buffer;
  buffer << cpu.format().first;
  buffer << " ";
  buffer << load.format().first;
  buffer << "   ";
  buffer << mem.format().first;
  buffer << "   ";
  buffer << net.format().first;
  buffer << "   ";
  buffer << uptime.format().first;
  return buffer.str();
}

remote_modules::remote_modules(const string hostname, const int width) : host(hostname),
                                                                         cpu(remote_wrapper("cat /proc/stat", hostname, unique_ptr<module>(new cpu_module("/tmp/" + hostname + ".stat", true, false, 2*width)))),
                                                                         mem(remote_wrapper("cat /proc/meminfo", hostname, unique_ptr<module>(new mem_module("/tmp/" + hostname + ".meminfo", 2*width)))),
                                                                         who(remote_wrapper("who", hostname, unique_ptr<module>(new who_module("/tmp/" + hostname + ".who")))),
                                                                         load(remote_wrapper("cat /proc/loadavg", hostname, unique_ptr<module>(new load_module("/tmp/" + hostname + ".load")))),
                                                                         uptime(remote_wrapper("cat /proc/uptime", hostname, unique_ptr<module>(new uptime_module("/tmp/" + hostname + ".uptime", 3)))) {
}

remote_modules::remote_modules(remote_modules&& other) : host(move(other.host)),
                                                         cpu(move(other.cpu)),
                                                         mem(move(other.mem)),
                                                         who(move(other.who)),
                                                         load(move(other.load)),
                                                         uptime(move(other.uptime)) {
}

void remote_modules::update() {
  cpu.update();
  mem.update();
  who.update();
  load.update();
  uptime.update();
}

pair<string, bool> remote_modules::format() const {
  bool backtrack = false;
  stringstream buffer;

  pair<string, bool> cpu_pair = cpu.format();
  buffer << cpu_pair.first;
  backtrack |= cpu_pair.second;
  buffer << " ";

  pair<string, bool> load_pair = load.format();
  buffer << load_pair.first;
  backtrack |= load_pair.second;
  buffer << "   ";

  pair<string, bool> mem_pair = mem.format();
  buffer << mem_pair.first;
  backtrack |= mem_pair.second;
  buffer << "   ";

  pair<string, bool> uptime_pair = uptime.format();
  buffer << uptime_pair.first;
  backtrack |= uptime_pair.second;
  buffer << "   ";

  pair<string, bool> who_pair = who.format();
  buffer << who_pair.first;
  backtrack |= who_pair.second;

  return make_pair(buffer.str(), backtrack);
}

const string remote_modules::hostname() const {
  return host;
}
