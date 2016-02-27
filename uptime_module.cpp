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

#include "uptime_module.hpp"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <ios>

using namespace std;

uptime_module::uptime_module(const string filename, const uint padding) : module(filename), days(0), padding(padding) {}

uptime_module::~uptime_module() {}

void uptime_module::update() {
  ifstream in(filename()[0]);
  uint seconds;
  in >> seconds;
  days = seconds / 60 / 60 / 24;
}

pair<string, bool> uptime_module::format() const {
  stringstream buffer;
  buffer << "up " << setw(padding) << setfill(' ') << days << " day" << (days == 1 ? " " : "s");
  return make_pair(buffer.str(), false);
}
