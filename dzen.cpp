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

#include "command_line_parser.hpp"
#include "modules.hpp"

#include <utility>
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

#ifdef DEBUG
    const uint delay_limit = 0;
#else
    const uint delay_limit = 2*hosts.size();
#endif

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

      while (true) {
        stringstream remote_buffer;
        bool backtrack = false;
        uint host_index = 0;

        for (remote_modules& host : hosts) {
          const string name = host.hostname();
          remote_buffer << name << ": ";

          for (uint i = 0; i < padding - name.size(); i++) {
            remote_buffer << " ";
          }

          if (delay + hosts.size() > delay_limit + host_index) {
            pair<string, bool> pair = host.format();

            if (pair.second) {
#ifdef DEBUG
              cerr << "\033[30mBacktracking ...\033[0m" << endl;
#endif
              backtrack = true;
              break;
            }

            remote_buffer << pair.first << endl;
          } else {
            remote_buffer << "<waiting for initial update>" << endl;
          }

          host_index++;
        }

        if (!backtrack) {
          buffer << remote_buffer.str();
          break;
        }

      } // while (true)

      if (steps >= pause_steps + hosts.size() - 1) {
        steps = 0;
      } else {
        steps++;
      }

      cout << buffer.str();
      flush(cout);
      offset = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start);

#ifdef DEBUG
      cerr << "\033[30mUpdate " << updated << " took " << offset.count() << " ms.\033[0m" << endl;
#endif

    } // while (true)

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

  key = vector<string>{"-a", "--aggregate"};
  bool net_aggregate = false;

  if (args.has(key)) {
    args.pop(key);
    net_aggregate = true;
  }

  args.warn();

  local_modules modules(width, net_aggregate);

  dzen(modules, move(remotes), pause_steps).run();

  return EXIT_SUCCESS;
}
