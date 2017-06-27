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

#ifndef MODULES_HPP
#define MODULES_HPP

#include "cpu_module.hpp"
#include "mem_module.hpp"
#include "who_module.hpp"
#include "load_module.hpp"
#include "remote_wrapper.hpp"
#include "net_module.hpp"
#include "battery_module.hpp"
#include "uptime_module.hpp"
#include "fuzzy_clock.hpp"

#include <string>
#include <utility>

class local_modules final {

public:
  local_modules(const int, const bool);
  void update();
  std::string format() const;

private:
  cpu_module cpu;
  mem_module mem;
  load_module load;
  net_module net;
  battery_module battery;
  uptime_module uptime;
  fuzzy_clock clock;

}; // class local_modules

class remote_modules final {

public:
  remote_modules(const std::string, const int);
  remote_modules(remote_modules&&);
  void update();
  std::pair<std::string, bool> format() const;
  const std::string hostname() const;

private:
  const std::string host;
  remote_wrapper cpu;
  remote_wrapper mem;
  remote_wrapper who;
  remote_wrapper load;
  remote_wrapper uptime;

}; // class remote_modules

#endif // MODULES_HPP
