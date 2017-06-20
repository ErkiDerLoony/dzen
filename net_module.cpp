/*
 * © Copyright 2015–2017 by Edgar Kalkowski <eMail@edgar-kalkowski.de>
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

#include "net_module.hpp"
#include "constants.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

net_module::net_module(string filename, const bool aggregate)
  : module(filename),
    aggregate(aggregate) {
}

net_module::~net_module() {}

net_module::net::net() : bytes(0),
                         packets(0),
                         errs(0),
                         drop(0),
                         fifo(0),
                         frame(0),
                         compressed(0),
                         multicast(0) {
}

istream& operator>>(istream& in, net_module::net_info& n) {
  in >> n.name
     >> n.rx.bytes >> n.rx.packets >> n.rx.errs >> n.rx.drop >> n.rx.fifo >> n.rx.frame >> n.rx.compressed >> n.rx.multicast
     >> n.tx.bytes >> n.tx.packets >> n.tx.errs >> n.tx.drop >> n.tx.fifo >> n.tx.frame >> n.tx.compressed >> n.tx.multicast;
  return in;
}

net_module::net_info operator-(const net_module::net_info& first, const net_module::net_info& second) {
  net_module::net_info result;
  result.name = first.name;
  result.rx.bytes = first.rx.bytes - second.rx.bytes;
  result.rx.packets = first.rx.packets - second.rx.packets;
  result.rx.errs = first.rx.errs - second.rx.errs;
  result.rx.drop = first.rx.drop - second.rx.drop;
  result.rx.fifo = first.rx.fifo - second.rx.fifo;
  result.rx.frame = first.rx.frame - second.rx.frame;
  result.rx.compressed = first.rx.compressed - second.rx.compressed;
  result.rx.multicast = first.rx.multicast - second.rx.multicast;
  result.tx.bytes = first.tx.bytes - second.tx.bytes;
  result.tx.packets = first.tx.packets - second.tx.packets;
  result.tx.errs = first.tx.errs - second.tx.errs;
  result.tx.drop = first.tx.drop - second.tx.drop;
  result.tx.fifo = first.tx.fifo - second.tx.fifo;
  result.tx.frame = first.tx.frame - second.tx.frame;
  result.tx.compressed = first.tx.compressed - second.tx.compressed;
  result.tx.multicast = first.tx.multicast - second.tx.multicast;
  return result;
}

void net_module::update() {

#ifdef DEBUG
  cerr << "\033[30mLoading " << filename()[0] << " ...\033[0m" << endl;
#endif

  ifstream in(filename()[0]);
  string line;
  map<string, net_module::net_info> nets;
  getline(in, line); // drop header
  getline(in, line);

  while (getline(in, line)) {
    net_module::net_info n;
    stringstream ss;
    ss << line;
    ss >> n;
    n.name = n.name.substr(0, n.name.length() - 1);

#ifdef DEBUG
    cerr << "\033[30mFound network interface " << n.name << " with bytes " << n.rx.bytes << ".\033[0m" << endl;
#endif

    nets[n.name] = n;
  }

  diffs.clear();

  for (auto net : nets) {
    auto item = previous.find(net.first);

    if (item != previous.end()) {
      diffs[net.first] = net.second - item->second;
#ifdef DEBUG
      cerr << "\033[30mdiff " << net.first << " = " << net.second.rx.bytes << " - " << item->second.rx.bytes << " = " << diffs[net.first].rx.bytes << "\033[0m" << endl;
#endif
    }

    if (diffs[net.first].rx.bytes > 0 || diffs[net.first].tx.bytes > 0) {
      active[net.first] = chrono::steady_clock::now();
    }
  }

  previous = nets;
}

void output(const unsigned long long value, stringstream& buffer) {
  const double mod = 1024.0;
  const long double kb = value / mod;
  const long double mb = kb / mod;

  if (mb < 1) {
    const unsigned long long digits = static_cast<unsigned long long>(log(kb)/log(10)) + 1;

    switch (digits) {
    case 3:
      buffer << setprecision(1) << kb << " KiB";
      break;
    case 2:
      buffer << setprecision(2) << kb << " KiB";
      break;
    default:
      buffer << setprecision(3) << kb << " KiB";
    }

  } else {
    const unsigned long long digits = static_cast<unsigned long long>(log(mb)/log(10)) + 1;

    switch (digits) {
    case 3:
      buffer << setprecision(1) << mb << " MiB";
      break;
    case 2:
      buffer << setprecision(2) << mb << " MiB";
      break;
    default:
      buffer << setprecision(3) << mb << " MiB";
    }
  }
}

pair<string, bool> net_module::format() const {
  stringstream buffer;
  buffer << fixed;
  uint counter = 0;

  if (aggregate) {
    unsigned long long total_rx = 0;
    unsigned long long total_tx = 0;

    for (pair<string, net_module::net_info> entry : diffs) {
#ifdef DEBUG
      cerr << "\033[30m" << entry.first << ": rx = " << entry.second.rx.bytes << ", tx = " << entry.second.tx.bytes << "\033[0m" << endl;
#endif
      total_rx += entry.second.rx.bytes;
      total_tx += entry.second.tx.bytes;
    }

#ifdef DEBUG
    cerr << "\033[30mtotal rx = " << total_rx << ", total tx = " << total_tx << endl;
#endif

    buffer << "Net ";
    buffer << "^fg(" << constants.green << ")";
    output(total_rx, buffer);
    buffer << "^fg() ";
    buffer << "^fg(" << constants.red << ")";
    output(total_tx, buffer);
    buffer << "^fg()";

  } else {
    chrono::steady_clock::time_point now = chrono::steady_clock::now();

    for (const pair<string, net_module::net_info> entry : diffs) {
      auto result = active.find(entry.first);
      const bool ever = result != active.end();
      chrono::steady_clock::time_point last = active.find(entry.first)->second;

#ifdef DEBUG
      auto offset = chrono::duration_cast<chrono::seconds>(now - last);

      if (ever) {
        cerr << "\033[30mNetwork " << entry.first << " was active " << offset.count() << " seconds ago.\033[0m" << endl;
      } else {
        cerr << "\033[30mNetwork " << entry.first << " was never active.\033[0m" << endl;
      }
#endif

      if (!ever || now - last > chrono::seconds(300)) {
        continue;
      }

      buffer << entry.first << " ";
      buffer << "^fg(" << constants.green << ")";
      output(entry.second.rx.bytes, buffer);
      buffer << "^fg() ";
      buffer << "^fg(" << constants.red << ")";
      output(entry.second.tx.bytes, buffer);
      buffer << "^fg()";

      if (counter < diffs.size() - 1) {
        buffer << "   ";
      }

      counter++;
    }
  }

  return make_pair(buffer.str(), false);
}
