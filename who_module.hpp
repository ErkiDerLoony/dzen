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

#ifndef WHO_MODULE_HPP
#define WHO_MODULE_HPP

#include "module.hpp"

#include <utility>
#include <string>
#include <set>
#include <map>

class who_module : public module {

public:
  who_module(const std::string, const std::string);
  virtual ~who_module();

  virtual void update();
  virtual std::pair<std::string, bool> format() const;

private:
  std::set<std::string> names;
  static std::map<std::vector<std::string>, uint> lengths;

  const uint max_length() const;

}; // class who_module

#endif // WHO_MODULE_HPP
