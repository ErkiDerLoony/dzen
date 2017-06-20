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

#ifndef NET_MODULE_HPP
#define NET_MODULE_HPP

#include "module.hpp"

#include <string>
#include <set>
#include <map>
#include <utility>
#include <chrono>

class net_module : public module {

public:
  net_module(std::string, const bool);
  virtual ~net_module();

  virtual void update();
  virtual std::pair<std::string, bool> format() const;

private:

  struct net {
    unsigned long long bytes;
    unsigned long long packets;
    unsigned long long errs;
    unsigned long long drop;
    unsigned long long fifo;
    unsigned long long frame;
    unsigned long long compressed;
    unsigned long long multicast;

    net();
  };

  struct net_info {
    std::string name;
    net rx;
    net tx;
  };

  std::map<std::string, net_info> previous;
  std::map<std::string, net_info> diffs;
  std::map<std::string, std::chrono::steady_clock::time_point> active;
  const bool aggregate;

  friend std::istream& operator>>(std::istream&, net_module::net_info&);
  friend net_info operator-(const net_info&, const net_info&);

}; // class net_module

#endif // NET_MODULE_HPP
