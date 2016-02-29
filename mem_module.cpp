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

#include "mem_module.hpp"
#include "constants.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

mem_module::mem_module(const string filename,
                       const int width) : module(filename),
                                          width(width) {
}

mem_module::~mem_module() {
}

void mem_module::update() {
  ifstream in(filename()[0]);

  uint done = 0;
  string line;

  while (done != 64-1 && getline(in, line)) {
    stringstream s(line);
    string name;
    long value;
    string unit;
    s >> name >> value >> unit;

    if (name == "MemTotal:") {
      state.total = value;
      done |= 1;
    } else if (name == "MemFree:") {
      state.free = value;
      done |= 2;
    } else if (name == "Cached:") {
      state.cached = value;
      done |= 4;
    } else if (name == "Buffers:") {
      state.buffers = value;
      done |= 8;
    } else if (name == "SwapTotal:") {
      state.swap_total = value;
      done |= 16;
    } else if (name == "SwapFree:") {
      state.swap_free = value;
      done |= 32;
    }
  }

  state.free = static_cast<long>(0.5 + width * state.free/state.total);
  state.buffers = static_cast<long>(0.5 + width * state.buffers/state.total);
  state.cached = static_cast<long>(0.5 + width * state.cached/state.total);
  state.total = width;

  if (state.swap_total != 0) {
    state.swap_free = static_cast<long>(0.5 + width * state.swap_free/state.swap_total);
    state.swap_total = width;
  } else {
    state.swap_free = width;
  }
}

void mem_module::output(stringstream& out) const {
  print(out, state.total - state.free - state.buffers - state.cached, constants.green);
  print(out, state.buffers, constants.blue);
  print(out, state.cached, constants.orange);
  print(out, state.free, constants.dark_grey);
  out << "^fg()";
}

void mem_module::output_swap(stringstream& out) const {

  if (state.swap_total == 0) {
    out << "^fg(" << constants.dark_grey << ")" << "^ro(" << state.swap_free << "x" << constants.height << ")" << "^fg()";
  } else {
    print(out, state.swap_total - state.swap_free, constants.red);
    print(out, state.swap_free, constants.dark_grey);
    out << "^fg()";
  }
}

pair<string, bool> mem_module::format() const {
  stringstream buffer;

  buffer << "RAM ";
  output(buffer);

  buffer << "   Swap ";
  output_swap(buffer);

  return make_pair(buffer.str(), false);
}
