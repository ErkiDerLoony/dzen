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

  dzen(local_modules local, vector<remote_modules> hosts, const uint pause_steps)
    : pause_steps(pause_steps), padding(calc_padding(hosts)), local(local), hosts(move(hosts)) {
  }

  void run() {
    uint steps = 0;
    chrono::steady_clock::time_point start;
    chrono::milliseconds offset(0);
    uint delay = 0;
    const uint delay_limit = hosts.size();

    local.update();

    while (true) {
      chrono::milliseconds pause = chrono::seconds(1) - offset;
      this_thread::sleep_for(pause);
      start = chrono::steady_clock::now();
      string updated = "(only local)";
      stringstream buffer;

      local.update();
      buffer << "^tw()" << local.format() << endl;

      if (steps < hosts.size()) {
        hosts[steps].update();
        updated = "for " + hosts[steps].hostname();

        if (delay < delay_limit) {
          delay++;
        }
      }

      offset = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start);

      if (offset.count() == 0) {
        debug << "Update " << updated << " took < 1 ms." << endl;
      } else {
        debug << "Update " << updated << " took " << offset.count() << " ms." << endl;
      }

      for (remote_modules& host : hosts) {
        const string name = host.hostname();
        buffer << name << ": ";

        for (uint i = 0; i < padding - name.size(); i++) {
          buffer << " ";
        }

        if (delay == delay_limit) {
          buffer << host.format() << endl;
        } else {
          buffer << "<waiting for initial update>" << endl;
        }
      }

      if (steps >= pause_steps + hosts.size() - 1) {
        steps = 0;
      } else {
        steps++;
      }

      cout << buffer.str();
      flush(cout);
    } // while(true)

  } // void run()

private:
  const uint pause_steps;
  const uint padding;
  local_modules local;
  vector<remote_modules> hosts;

  static inline uint calc_padding(const vector<remote_modules>& hosts) {
    uint max_length = 0;

    for (const remote_modules& host : hosts) {
      uint length = host.hostname().length();

      if (length > max_length) {
        max_length = length;
      }
    }

    return max_length;
  }

}; // class dzen

int main(int argc, char** argv) {
  command_line_parser args(argc, argv);

  if (args.has({"-h", "--help"})) {
    cerr << "Usage: dzen [--width <PIXELS>] [--help] [--remotes <HOSTNAMES>]" << endl;
    return EXIT_SUCCESS;
  }

  int width = 100;
  vector<string> key({"-w", "--width"});

  if (args.has(key)) {
    optional<string> arg = args.pop(key);

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
  key = vector<string>{"-r", "--remote", "--remotes"};

  if (args.has(key)) {
    optional<string> arg = args.pop(key);

    if (arg) {
      istringstream s(arg.value());
      string remote;

      while (getline(s, remote, ',')) {
        const string host(remote);

        if (remotes.size() < 15) {
          remotes.push_back(remote_modules(host, width));
        } else {
          cerr << "Only 10 remotes are supported! Ignoring " << host << "!" << endl;
        }
      }

    } else {
      cerr << "You must specify at least one remote (or multiple remotes separated by commas)!" << endl;
      return EXIT_FAILURE;
    }
  }

  key = vector<string>{"-p", "--pause"};
  uint pause_steps = 300;

  if (args.has(key)) {
    optional<string> arg = args.pop(key);

    if (arg) {
      istringstream s(arg.value());
      s >> pause_steps;
    } else {
      cerr << "Yous must specify a pause in steps!" << endl;
      return EXIT_FAILURE;
    }
  }

  args.warn();

  local_modules modules(width);

  dzen(modules, move(remotes), pause_steps).run();

  return EXIT_SUCCESS;
}
