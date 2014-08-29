#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>

using namespace std;

struct cpu {
  long user;
  long nice;
  long system;
  long idle;
  long iowait;
  long irq;
  long softirq;
  long steal;
  long guest;
  long guest_nice;
};

istream& operator>>(istream& stream, cpu& cpu) {
  stream >> cpu.user >> cpu.nice >> cpu.system >> cpu.idle >> cpu.iowait >> cpu.irq >> cpu.softirq >> cpu.steal >> cpu.guest >> cpu.guest_nice;
  return stream;
}

ostream& operator<<(ostream& stream, const cpu& cpu) {
  stream << "user: " << cpu.user
         << ", nice: " << cpu.nice
         << ", system: " << cpu.system
         << ", idle: " << cpu.idle
         << ", iowait: " << cpu.iowait
         << ", irq: " << cpu.irq
         << ", softirq: " << cpu.softirq
         << ", steal: " << cpu.steal
         << ", guest: " << cpu.guest
         << ", guest_nice: " << cpu.guest_nice;
  return stream;
}

cpu operator-(const cpu& minuend, const cpu& subtrahend) {
  cpu result;
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

struct cpu_info {
  cpu total;
  vector<cpu> cpus;
};

cpu_info operator-(const cpu_info& first, const cpu_info& second) {
  cpu_info result;
  result.total = first.total - second.total;
  auto it = first.cpus.begin();
  auto oit = second.cpus.begin();

  while (it != first.cpus.end() && oit != second.cpus.end()) {
    result.cpus.push_back(*(it++) - *(oit++));
  }

  return result;
}

cpu_info read_cpu_info(string filename) {
  ifstream in(filename);

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

  return cpu_info{total, cpus};
}

cpu_info read_remote_cpu_info(const string& remote) {
  string filename("/tmp/" + remote + ".proc_stat");
  system(string("scp " + remote + ":/proc/stat " + filename + " &>/dev/null").c_str());
  cpu_info result = read_cpu_info(filename);
  remove(filename.c_str());
  return result;
}

class dzen final {

public:

  dzen(const int width, const vector<string> remotes) : width(width), remotes(remotes) {
  }

  void run() {
    bool initialized = false;
    cpu_info before(read_cpu_info("/proc/stat"));
    vector<pair<string, cpu_info>> remote;

    for (string host : remotes) {
      remote.push_back(make_pair(host, read_remote_cpu_info(host)));
    }

    while (true) {
      this_thread::sleep_for(chrono::seconds(1));

      // Process local CPU information.
      cpu_info after(read_cpu_info("/proc/stat"));
      cpu_info diff(after - before);
      cout << "CPUs";

      for (auto cpu : diff.cpus) {
        cout << " " << print(cpu);
      }

      cout << endl;
    }
  }

private:
  const int width;
  const vector<string> remotes;

  static const int HEIGHT = 10;

  static const string BLUE;
  static const string GREEN;
  static const string ORANGE;
  static const string RED;
  static const string CYAN;
  static const string MAGENTA;
  static const string GREY;
  static const string DARK_GREY;

  inline void p(ostream& stream, const long& value, const string& colour) {
    stream << "^fg(" << colour << ")^r(" << value << "x" << HEIGHT << ")";
  }

  string print(const cpu& c) {
    cpu n = normalize(c);
    stringstream s;
    p(s, n.nice, BLUE);
    p(s, n.user, GREEN);
    p(s, n.system, RED);
    p(s, n.irq, ORANGE);
    p(s, n.softirq, MAGENTA);
    p(s, n.iowait, GREY);
    p(s, n.steal, CYAN);
    p(s, n.guest, CYAN);
    p(s, n.guest_nice, CYAN);
    p(s, n.idle, DARK_GREY);
    return s.str();
  }

  inline long sum(const cpu& cpu) const {
    return cpu.user + cpu.nice + cpu.system + cpu.idle + cpu.iowait + cpu.irq + cpu.softirq + cpu.steal + cpu.guest + cpu.guest_nice;
  }

  cpu normalize(const cpu& c) const {
    cpu result;
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
};

const string dzen::BLUE = "#0078bf";
const string dzen::GREEN = "#00bf00";
const string dzen::ORANGE = "#bf9900";
const string dzen::RED = "#b62300";
const string dzen::CYAN = "#00bfbf";
const string dzen::MAGENTA = "#b400a3";
const string dzen::GREY = "#969696";
const string dzen::DARK_GREY = "#444444";

int main(int argc, char** argv) {

  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <width>" << endl;
    return EXIT_SUCCESS;
  }

  stringstream buffer;
  buffer << argv[1];
  long width;
  buffer >> width;

  vector<string> remotes{"pallando", "alatar"};
  dzen(width, remotes).run();

  return EXIT_SUCCESS;
}
