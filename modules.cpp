/*
 * © Copyright 2015–2016 by Edgar Kalkowski <eMail@edgar-kalkowski.de>
 *
 * This file is part of the dzen2 config program dzen++.
 *
 * The dzen2 config program dzen++ is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program. If
 * not, see <http://www.gnu.org/licenses/>.
 */

#include "modules.hpp"

#include <sstream>

using namespace std;

local_modules::local_modules(const int width, const bool aggregate)
  : cpu("/proc/stat", false, true, static_cast<int>(0.75*width)),
    mem("/proc/meminfo", width),
    load("/proc/loadavg", "/proc/cpuinfo"),
    net("/proc/net/dev", aggregate),
    battery("/sys/class/power_supply/BAT0/capacity", width),
    uptime("/proc/uptime", 1) {
}

void local_modules::update() {
  cpu.update();
  mem.update();
  load.update();
  net.update();
  battery.update();
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
  buffer << battery.format().first;
  buffer << "   ";
  buffer << net.format().first;
  buffer << "   ";
  buffer << uptime.format().first;
  return buffer.str();
}

remote_modules::remote_modules(const string hostname, const int width)
  : host(hostname),
    cpu(remote_wrapper(vector<string>{"cat /proc/stat"}, hostname, unique_ptr<module>(new cpu_module("/tmp/" + hostname + ".stat", true, false, 2*width)))),
    mem(remote_wrapper(vector<string>{"cat /proc/meminfo"}, hostname, unique_ptr<module>(new mem_module("/tmp/" + hostname + ".meminfo", 2*width)))),
    who(remote_wrapper(vector<string>{"who", "ps --no-header -e -o %mem,%cpu,euser"}, hostname, unique_ptr<module>(new who_module("/tmp/" + hostname + ".who", "/tmp/" + hostname + ".who2")))),
    load(remote_wrapper(vector<string>{"cat /proc/loadavg", "cat /proc/cpuinfo"}, hostname, unique_ptr<module>(new load_module("/tmp/" + hostname + ".load", "/tmp/" + hostname + ".cpuinfo")))),
    uptime(remote_wrapper(vector<string>{"cat /proc/uptime"}, hostname, unique_ptr<module>(new uptime_module("/tmp/" + hostname + ".uptime", 3)))) {
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
