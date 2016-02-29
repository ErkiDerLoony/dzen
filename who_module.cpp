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

#include "who_module.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

map<vector<string>, uint> who_module::lengths;

who_module::who_module(const string filename, const string filename2) : module({filename, filename2}) {}

who_module::~who_module() {}

ostream& operator<<(ostream& out, const set<string>& set) {
  auto it = set.begin();
  out << "{";

  for (uint i = 0; i < set.size(); i++) {
    out << *it;
    it++;

    if (i < set.size() - 1) {
      out << ", ";
    }
  }

  out << "}";
  return out;
}

void who_module::update() {
  ifstream in(filename()[0]);
  string line;
  names.clear();

  while (getline(in, line)) {
    stringstream s(line);
    string name;
    s >> name;
    names.insert(name);
  }

  ifstream in2(filename()[1]);

  while (getline(in2, line)) {
    stringstream s(line);
    double mem;
    s >> mem;
    double cpu;
    s >> cpu;
    string name;
    s >> name;

    if (cpu > 10.0 || mem > 1.0) {
      names.insert(name);
    }
  }
}

pair<string, bool> who_module::format() const {
  uint count = 0;
  stringstream s;
  bool backtrack = false;

  for (const string name : names) {
    s << name;

    if (count < names.size() - 1) {
      s << ", ";
    }

    count++;
  }

  const uint pre_len = s.str().length();

  if (pre_len != lengths[filename()]) {
    backtrack = true;
  }

  lengths[filename()] = pre_len;
  const uint max_len = max_length();

  if (pre_len < max_len) {

    for (uint i = 0; i < max_len - pre_len; i++) {
      s << " ";
    }
  }

  return make_pair(s.str(), backtrack);
}

const uint who_module::max_length() const {
  uint result = 0;

  for (const pair<vector<string>, uint>& entry : lengths) {

    if (entry.second > result) {
      result = entry.second;
    }
  }

  return result;
}
