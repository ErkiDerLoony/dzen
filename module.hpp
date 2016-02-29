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

#ifndef MODULE_HPP
#define MODULE_HPP

#include <initializer_list>
#include <vector>
#include <string>
#include <ostream>

class module {

public:

  /**
   * Create a new module.
   *
   * @param filename  Name of the file this module reads. In case of remote
   *                  modules this filename is pre-processed by
   *                  {@link remote_wrapper} and the remote file is transmitted
   *                  to a local file which is then handed to the actual
   *                  underlying module for further processing.
   */
  module(std::string filename);

  /**
   * Create a new module that requires multiple files.
   *
   * @param files  Names of files this module requires. In case of a remote
   *               module those are automatically transmitted by
   *               {@link remote_wrapper} and the suitable local filename is
   *               handed to the actual underlying module for processing.
   */
  module(const std::initializer_list<std::string> files);

  /**
   * Create a new module that requires multiple files.
   *
   * @param files  Names of files this module requires. In case of a remote
   *               module those are automatically transmitted by
   *               {@link remote_wrapper} and the suitable local filename is
   *               handed to the actual underlying module for processing.
   */
  module(const std::vector<std::string> files);

  virtual ~module();

  std::vector<std::string> filename() const;

  virtual void update() = 0;

  /**
   * Format the output of this module suitable for consumption by dzen2.
   *
   * @return  A pair that contains the formatted output as the first item and a
   *          boolean flag that indicates backtracking as the second item. If
   *          the backtracking flag is {@code true} output of all remote modules
   *          is repeated once more. This allows for a better alignment of the
   *          output of remote modules.
   */
  virtual std::pair<std::string, bool> format() const = 0;

protected:
  void print(std::ostream&, const long&, const std::string&) const;

private:
  const std::vector<std::string> files;

};

#endif // MODULE_HPP
