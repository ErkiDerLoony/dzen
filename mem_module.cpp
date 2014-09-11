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
  ifstream in(filename());

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
  print(out, state.swap_total - state.swap_free, constants.red);
  print(out, state.swap_free, constants.dark_grey);
  out << "^fg()";
}

string mem_module::format() const {
  stringstream buffer;

  buffer << "RAM ";
  output(buffer);

  if (state.swap_total != 0) {
    buffer << "   Swap ";
    output_swap(buffer);
  }

  return buffer.str();
}
