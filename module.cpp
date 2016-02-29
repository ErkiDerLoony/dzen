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

#include "module.hpp"
#include "constants.hpp"

using namespace std;

module::module(const string filename) : module({filename}) {
}

module::module(const initializer_list<string> filenames) : files(filenames) {
}

module::module(const vector<string> filenames) : files(filenames) {
}

module::~module() {
}

vector<string> module::filename() const {
  return files;
}

void module::print(std::ostream& stream, const long& value, const string& colour) const {
  stream << "^fg(" << colour << ")^r(" << value << "x" << constants.height << ")";
}
