#include "cpu_module.hpp"

#include <fstream>
#include <sstream>

using namespace std;

cpu_module::cpu_module(const string filename) : module(filename) {
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

  previous = state;
  state = cpu_module::cpu_info(total, cpus);
}

string cpu_module::format() const {
  stringstream buffer;
  buffer << (state - previous);
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

ostream& operator<<(ostream& stream, const cpu_module::cpu_info& info) {
  stream << "CPU " << info.total.idle;
  return stream;
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
