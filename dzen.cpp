#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <thread>
#include <chrono>
#include <experimental/optional>

using namespace std;
using experimental::optional;

class log final {

public:

  template<typename T>
  log& operator<<(const T& text) {
#ifdef DEBUG
    buffer << text;
#endif
    return *this;
  }

  typedef basic_ostream<char, char_traits<char>> endl_t;
  log& operator<<(endl_t&(*endl)(endl_t&)) {
#ifdef DEBUG
    clog << "\033[30m" << buffer.str() << "\033[m" << endl;
    buffer.str("");
    buffer.clear();
#endif
    return *this;
  }

private:
  stringstream buffer;

};

log debug;

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

optional<cpu_info> read_remote_cpu_info(const string& remote) {
  string filename("/tmp/" + remote + ".proc_stat");
  const int result = system(string("ssh " + remote + " 'cat /proc/stat' > " + filename + " 2>/dev/null").c_str());

  if (result == 0) {
    cpu_info info = read_cpu_info(filename);
    remove(filename.c_str());
    return optional<cpu_info>(info);
  } else {
    return optional<cpu_info>();
  }
}

class dzen final {

public:

  dzen(const int width, const vector<string> hosts) : width(width), hosts(hosts), padding(calc_padding(hosts)) {
  }

  void run() {
    int steps = 0;
    const int limit = 10;
    chrono::steady_clock::time_point start;
    chrono::milliseconds offset(0);
    cpu_info before(read_cpu_info("/proc/stat"));
    map<string, optional<cpu_info>> remotes;
    map<string, optional<cpu_info>> remote_diffs;

    for (const string host : hosts) {
      remotes[host] = read_remote_cpu_info(host);
    }

    while (true) {
      chrono::milliseconds sleep = chrono::seconds(1) - offset;
      this_thread::sleep_for(sleep);
      start = chrono::steady_clock::now();
      steps++;

      // Update local CPU information.
      cpu_info after(read_cpu_info("/proc/stat"));
      cpu_info diff(after - before);
      before = after;
      cout << "^tw()CPUs";

      for (auto cpu : diff.cpus) {
        cout << " " << print(cpu, static_cast<long>(0.75*width));
      }

      cout << endl;

      // Update remote CPU information.
      if (steps >= limit || remote_diffs.empty()) {
        steps = 0;

        for (const string host : hosts) {
          optional<cpu_info> now(read_remote_cpu_info(host));

          if (now && remotes[host]) {
            cpu_info n = now.value_or(cpu_info());
            cpu_info b = remotes[host].value_or(cpu_info());
            remote_diffs[host] = optional<cpu_info>(n - b);
          } else {
            remote_diffs.erase(host);
          }

          if (now) {
            remotes[host] = now;
          }
        }

        stringstream buffer;
        buffer << "^cs()" << endl;

        for (const string host : hosts) {
          buffer << host << ": ";

          for (uint i = 0; i < padding - host.length(); i++) {
            buffer << " ";
          }

          if (remote_diffs.find(host) != remote_diffs.end()) {
            buffer << "CPU " << print(remote_diffs[host].value_or(cpu_info()).total, 2 * width) << endl;
          } else {
            buffer << "<unreachable>" << endl;
          }
        }

        cout << buffer.str();
      }

      offset = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start);

      if (offset.count() == 0) {
        debug << "Update took < 1 ms." << endl;
      } else {
        debug << "Update took " << offset.count() << " ms." << endl;
      }
    }
  }

private:
  const int width;
  const vector<string> hosts;
  const int padding;

  static const int HEIGHT = 10;

  static const string BLUE;
  static const string GREEN;
  static const string ORANGE;
  static const string RED;
  static const string CYAN;
  static const string MAGENTA;
  static const string GREY;
  static const string DARK_GREY;

  static int calc_padding(const vector<string>& hosts) {
    int max_length = 0;

    for (string host : hosts) {
      int length = host.length();

      if (length > max_length) {
        max_length = length;
      }
    }

    return max_length;
  }

  static inline void p(ostream& stream, const long& value, const string& colour) {
    stream << "^fg(" << colour << ")^r(" << value << "x" << HEIGHT << ")";
  }

  /**
   * Format a CPU info instance according to the format dzen2 expects.
   *
   * @param cpu  CPU info instance to be formatted.
   * @param width  Width in pixels in which the specified CPU info instance shall be visualized.
   * @return  A string that contains output suitable for dzen2.
   */
  static string print(const cpu& c, const long width) {
    cpu n = normalize(c, width);
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

  static inline long sum(const cpu& cpu) {
    return cpu.user + cpu.nice + cpu.system + cpu.idle + cpu.iowait + cpu.irq + cpu.softirq + cpu.steal + cpu.guest + cpu.guest_nice;
  }

  /**
   * Normalize a CPU info instance to a desired display width.
   *
   * @param c  CPU info instance to be normalized.
   * @param width  Width in pixels the given CPU info instance shall be normalized to.
   * @return  A new CPU info instance that is normalized in such a way that the sum of all it’s entries equals the specified width.
   */
  static cpu normalize(const cpu& c, const long& width) {
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

  vector<string> remotes{"pallando", "alatar", "radagast", "sauron", "ulmo", "irmo", "melko"};
  dzen(width, remotes).run();

  return EXIT_SUCCESS;
}
