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

#ifndef CPU_MODULE
#define CPU_MODULE

#include "module.hpp"

#include <string>
#include <vector>
#include <iostream>

class cpu_module : public module {

public:
  cpu_module(const std::string, const bool total, const bool parts, const int width);
  virtual ~cpu_module();
  virtual void update();
  virtual std::pair<std::string, bool> format() const;

private:

  struct cpu {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
    unsigned long steal;
    unsigned long guest;
    unsigned long guest_nice;

    cpu();
  };

  struct cpu_info {
    cpu total;
    std::vector<cpu> cpus;

    cpu_info(const cpu, const std::vector<cpu>);
    cpu_info();
    void operator=(const cpu_info& other);
  };

  cpu_info previous;
  cpu_info diff;
  const bool total;
  const bool parts;
  const int width;

  void output(const cpu&, std::stringstream&) const;
  inline long sum(const cpu&) const;
  cpu normalize(const cpu&) const;

  friend cpu operator-(const cpu&, const cpu&);
  friend cpu_info operator-(const cpu_info&, const cpu_info&);
  friend std::istream& operator>>(std::istream&, cpu_module::cpu&);
  friend std::ostream& operator<<(std::ostream&, const cpu_module::cpu_info&);

}; // class cpu_module

#endif // CPU_MODULE
