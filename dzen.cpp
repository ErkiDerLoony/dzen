#include "command_line_parser.hpp"
#include "log.hpp"
#include "modules.hpp"

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

class dzen final {

public:

  dzen(const int width, local_modules local, vector<remote_modules> hosts)
    : width(width), local(local), hosts(move(hosts)) {
  }

  void run() {
    uint steps = 0;
    chrono::steady_clock::time_point start;
    chrono::milliseconds offset(0);
    // cpu_info before(read_cpu_info("/proc/stat"));
    // map<string, optional<cpu_info>> remotes;
    // map<string, optional<cpu_info>> remote_diffs;

    // for (const string host : hosts) {
    //   remotes[host] = read_remote_cpu_info(host);
    // }

    local.update();

    for (remote_modules& host : hosts) {
      host.update();
    }

    while (true) {
      chrono::milliseconds sleep = chrono::seconds(1) - offset;
      this_thread::sleep_for(sleep);
      start = chrono::steady_clock::now();

      local.update();
      cout << local.format() << endl;

      for (remote_modules& host : hosts) {
        host.update();
        cout << host.format() << endl;
      }

    //   // Update local CPU info.
    //   cpu_info after(read_cpu_info("/proc/stat"));
    //   cpu_info diff(after - before);
    //   before = after;

    //   // Print local CPU info.
    //   cout << "^tw()CPUs";

    //   for (auto cpu : diff.cpus) {
    //     cout << " " << print(cpu, static_cast<long>(0.75*width));
    //   }

    //   cout << endl;

    //   // Update CPU info of one remote host.
    //   if (!hosts.empty()) {
    //     const string host = hosts[steps];
    //     optional<cpu_info> now(read_remote_cpu_info(host));

    //     if (now && remotes[host]) {
    //       cpu_info n = now.value_or(cpu_info());
    //       cpu_info b = remotes[host].value_or(cpu_info());
    //       remote_diffs[host] = optional<cpu_info>(n - b);
    //     } else {
    //       remote_diffs.erase(host);
    //     }

    //     if (now) {
    //       remotes[host] = now;
    //     }

    //     // Print remote CPU info.
    //     stringstream buffer;

    //     for (const string host : hosts) {
    //       buffer << host << ": ";

    //       for (uint i = 0; i < padding - host.length(); i++) {
    //         buffer << " ";
    //       }

    //       if (remote_diffs.find(host) != remote_diffs.end()) {
    //         buffer << "CPU " << print(remote_diffs[host].value_or(cpu_info()).total, 2 * width) << endl;
    //       } else {
    //         buffer << "<unreachable>" << endl;
    //       }
    //     }

    //     cout << buffer.str();
    //   }

      offset = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start);

      if (offset.count() == 0) {
        debug << "Update took < 1 ms." << endl;
      } else {
        debug << "Update took " << offset.count() << " ms." << endl;
      }

      // if (steps == hosts.size() - 1) {
      //   steps = 0;
      // } else {
      //   steps++;
      // }
    }
  }

private:
  const int width;
  local_modules local;
  vector<remote_modules> hosts;

  // static int calc_padding(const vector<remote_modules>& hosts) {
  //   int max_length = 0;

  //   for (const remote_modules& host : hosts) {
  //     int length = host.hostname().length();

  //     if (length > max_length) {
  //       max_length = length;
  //     }
  //   }

  //   return max_length;
  // }

  // static inline void p(ostream& stream, const long& value, const string& colour) {
  //   stream << "^fg(" << colour << ")^r(" << value << "x" << HEIGHT << ")";
  // }

  /**
   * Format a CPU info instance according to the format dzen2 expects.
   *
   * @param cpu  CPU info instance to be formatted.
   * @param width  Width in pixels in which the specified CPU info instance shall be visualized.
   * @return  A string that contains output suitable for dzen2.
   */
  // static string print(const cpu& c, const long width) {
  //   cpu n = normalize(c, width);
  //   stringstream s;
  //   p(s, n.nice, BLUE);
  //   p(s, n.user, GREEN);
  //   p(s, n.system, RED);
  //   p(s, n.irq, ORANGE);
  //   p(s, n.softirq, MAGENTA);
  //   p(s, n.iowait, GREY);
  //   p(s, n.steal, CYAN);
  //   p(s, n.guest, CYAN);
  //   p(s, n.guest_nice, CYAN);
  //   p(s, n.idle, DARK_GREY);
  //   return s.str();
  // }

  // static inline long sum(const cpu& cpu) {
  //   return cpu.user + cpu.nice + cpu.system + cpu.idle + cpu.iowait + cpu.irq + cpu.softirq + cpu.steal + cpu.guest + cpu.guest_nice;
  // }

  /**
   * Normalize a CPU info instance to a desired display width.
   *
   * @param c  CPU info instance to be normalized.
   * @param width  Width in pixels the given CPU info instance shall be normalized to.
   * @return  A new CPU info instance that is normalized in such a way that the sum of all it’s entries equals the specified width.
   */
  // static cpu normalize(const cpu& c, const long& width) {
  //   cpu result;
  //   const long s = sum(c);
  //   result.user = width * c.user / s;
  //   result.nice = width * c.nice / s;
  //   result.system = width * c.system / s;
  //   result.idle = 0;
  //   result.iowait = width * c.iowait / s;
  //   result.irq = width * c.irq / s;
  //   result.softirq = width * c.softirq / s;
  //   result.steal = width * c.steal / s;
  //   result.guest = width * c.guest / s;
  //   result.guest_nice = width * c.guest_nice / s;
  //   result.idle = width - sum(result);
  //   return result;
  // }
};

int main(int argc, char** argv) {
  command_line_parser args(argc, argv);

  if (args.has({"--help"})) {
    cerr << "Usage: dzen [--width <PIXELS>] [--help] [--remotes <HOSTNAMES>]" << endl;
    return EXIT_SUCCESS;
  }

  long width = 100;

  if (args.has({"--width"})) {
    optional<string> arg = args.pop({"--width"});

    if (arg) {
      stringstream buffer;
      buffer << arg.value();
      buffer >> width;
    } else {
      cerr << "You must specify a width in pixels!" << endl;
      return EXIT_FAILURE;
    }
  }

  vector<remote_modules> remotes;

  if (args.has({"--remotes"})) {
    optional<string> arg = args.pop({"--remotes"});

    if (arg) {
      istringstream s(arg.value());
      string remote;

      while (getline(s, remote, ',')) {
        const string host(remote);
        remotes.push_back(remote_modules(host));
      }

    } else {
      cerr << "You must specify at least one remote (or multiple remotes separated by commas)!" << endl;
      return EXIT_FAILURE;
    }
  }

  args.warn();

  local_modules modules;

  dzen(width, modules, move(remotes)).run();

  return EXIT_SUCCESS;
}
