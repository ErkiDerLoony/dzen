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

#ifndef MEM_MODULE
#define MEM_MODULE

#include "module.hpp"

#include <utility>
#include <string>
#include <sstream>

class mem_module : public module {

public:
  mem_module(const std::string, const int);
  virtual ~mem_module();
  virtual void update();
  virtual std::pair<std::string, bool> format() const;

private:

  struct mem_info {
    ulong total;
    ulong free;
    ulong buffers;
    ulong cached;
    ulong swap_total;
    ulong swap_free;
  };

  mem_info state;
  const int width;

  void output(std::stringstream&) const;
  void output_swap(std::stringstream&) const;

}; // class mem_module

#endif // MEM_MODULE
