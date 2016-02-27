/*
 * © Copyright 2015–2016 by Edgar Kalkowski <eMail@edgar-kalkowski.de>
 *
 * This file is part of the dzen2 config program dzen.
 *
 * The dzen2 config program dzen is free software; you can redistribute it and/or modify it under
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

#include "remote_wrapper.hpp"

#include <iostream>

using namespace std;

remote_wrapper::remote_wrapper(const vector<string> remote_commands, const string hostname, unique_ptr<module> inner)
  : module(inner->filename()),
    ok(false),
    remote_commands(remote_commands),
    hostname(hostname),
    inner(move(inner)) {
}

remote_wrapper::remote_wrapper(remote_wrapper&& other)
  : module(other.inner->filename()),
    ok(false),
    remote_commands(other.remote_commands),
    hostname(other.hostname) {
  inner = move(other.inner);
}

remote_wrapper::~remote_wrapper() {
}

void remote_wrapper::update() {
  uint index = 0;

  for (string filename : inner->filename()) {
    const int result = system(string("ssh -o ConnectTimeout=5 " + hostname + " '" + remote_commands[index++] + "' > " + filename + " 2>/dev/null").c_str());

    if (result == 0) {
      ok = true;
    } else {
      ok = false;
      break;
    }
  }

  if (ok) {
    inner->update();
  }

  for (string filename : inner->filename()) {
    remove(filename.c_str());
  }
}

pair<string, bool> remote_wrapper::format() const {

  if (ok) {
    return inner->format();
  } else {
    return make_pair("<unreachable>", false);
  }
}
