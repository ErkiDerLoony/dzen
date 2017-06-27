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

#ifndef FUZZY_CLOCK_HPP
#define FUZZY_CLOCK_HPP

#include "module.hpp"

#include <chrono>

class fuzzy_clock : public module {

public:
  fuzzy_clock();
  virtual ~fuzzy_clock();

  virtual void update();
  virtual std::pair<std::string, bool> format() const;

  std::string format(const int, const int, const int) const;

private:
  std::string format(const int) const;

};

#endif // FUZZY_CLOCK_HPP
