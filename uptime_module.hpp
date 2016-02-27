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

#ifndef UPTIME_MODULE_HPP
#define UPTIME_MODULE_HPP

#include "module.hpp"

#include <string>
#include <utility>

class uptime_module : public module {

public:
  uptime_module(const std::string, const uint);
  virtual ~uptime_module();

  virtual void update();
  virtual std::pair<std::string, bool> format() const;

private:
  uint days;
  const uint padding;

}; // class uptime_module

#endif // UPTIME_MODULE_HPP
