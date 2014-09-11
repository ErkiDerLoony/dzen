#include "cpu_module.hpp"
#include "constants.hpp"

#include <fstream>
#include <sstream>

using namespace std;

cpu_module::cpu_module(const string filename,
                       const bool total,
                       const bool parts,
                       const int width) : module(filename),
                                          total(total),
                                          parts(parts),
                                          width(width) {
}

cpu_module::~cpu_module() {
}

void cpu_module::update() {
  ifstream in(filename());

  string line;
  vector<cpu> cpus;
  cpu total;

  while (getline(in, line)) {

    if (line.substr(0, 4) == "cpu ") {
      stringstream s(line);
      string name;
      s >> name >> total;
    } else if (line.substr(0, 3) == "cpu") {
      stringstream s(line);
      string name;
      cpu c;
      s >> name >> c;
      cpus.push_back(c);
    } else {
      break;
    }
  }

  cpu_module::cpu_info state(total, cpus);
  diff = state - previous;
  previous = state;
}

long cpu_module::sum(const cpu_module::cpu& cpu) const {
  return cpu.user + cpu.nice + cpu.system + cpu.idle + cpu.iowait + cpu.irq + cpu.softirq + cpu.steal + cpu.guest + cpu.guest_nice;
}

cpu_module::cpu cpu_module::normalize(const cpu_module::cpu& c) const {
  cpu_module::cpu result;
  const long s = sum(c);
  result.user = width * c.user / s;
  result.nice = width * c.nice / s;
  result.system = width * c.system / s;
  result.idle = 0;
  result.iowait = width * c.iowait / s;
  result.irq = width * c.irq / s;
  result.softirq = width * c.softirq / s;
  result.steal = width * c.steal / s;
  result.guest = width * c.guest / s;
  result.guest_nice = width * c.guest_nice / s;
  result.idle = width - sum(result);
  return result;
}

inline void p(ostream& stream, const long& value, const string& colour) {
  stream << "^fg(" << colour << ")^r(" << value << "x" << constants.height << ")";
}

void cpu_module::print(const cpu& diff, stringstream& buffer) const {
  const cpu_module::cpu n = normalize(diff);
  p(buffer, n.nice, constants.blue);
  p(buffer, n.user, constants.green);
  p(buffer, n.system, constants.red);
  p(buffer, n.irq, constants.orange);
  p(buffer, n.softirq, constants.magenta);
  p(buffer, n.iowait, constants.grey);
  p(buffer, n.steal, constants.cyan);
  p(buffer, n.guest, constants.cyan);
  p(buffer, n.guest_nice, constants.cyan);
  p(buffer, n.idle, constants.dark_grey);
}

string cpu_module::format() const {
  stringstream buffer;

  if (total) {
    buffer << "CPU ";
    print(diff.total, buffer);
  }

  if (parts) {
    buffer << "CPUs ";

    for (uint i = 0; i < diff.cpus.size(); i++) {
      print(diff.cpus[i], buffer);

      if (i < diff.cpus.size() - 1) {
        buffer << " ";
      }
    }
  }

  return buffer.str();
}

cpu_module::cpu_info::cpu_info(const cpu total, const vector<cpu> cpus) : total(total), cpus(cpus) {
}

cpu_module::cpu_info::cpu_info() {
}

void cpu_module::cpu_info::operator=(const cpu_info& other) {
  total = other.total;
  cpus = other.cpus;
}

cpu_module::cpu_info operator-(const cpu_module::cpu_info& minuend,
                               const cpu_module::cpu_info& subtrahend) {
  const cpu_module::cpu total = minuend.total - subtrahend.total;
  vector<cpu_module::cpu> cpus;
  auto it = minuend.cpus.begin();
  auto oit = subtrahend.cpus.begin();

  while (it != minuend.cpus.end() && oit != subtrahend.cpus.end()) {
    cpus.push_back(*(it++) - *(oit++));
  }

  return cpu_module::cpu_info(total, cpus);
}

cpu_module::cpu::cpu() : user(0), nice(0), system(0), idle(0), iowait(0),
                         irq(0), softirq(0), steal(0), guest(0), guest_nice(0) {
}

istream& operator>>(istream& stream, cpu_module::cpu& cpu) {
  stream >> cpu.user >> cpu.nice >> cpu.system >> cpu.idle >> cpu.iowait
         >> cpu.irq >> cpu.softirq >> cpu.steal >> cpu.guest >> cpu.guest_nice;
  return stream;
}

cpu_module::cpu operator-(const cpu_module::cpu& minuend, const cpu_module::cpu& subtrahend) {
  cpu_module::cpu result;
  result.user = minuend.user - subtrahend.user;
  result.nice = minuend.nice - subtrahend.nice;
  result.system = minuend.system - subtrahend.system;
  result.idle = minuend.idle - subtrahend.idle;
  result.iowait = minuend.iowait - subtrahend.iowait;
  result.irq = minuend.irq - subtrahend.irq;
  result.softirq = minuend.softirq - subtrahend.softirq;
  result.steal = minuend.steal - subtrahend.steal;
  result.guest = minuend.guest - subtrahend.guest;
  result.guest_nice = minuend.guest_nice - subtrahend.guest_nice;
  return result;
}
