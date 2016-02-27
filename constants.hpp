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

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

struct {
  const int height = 10;

  const std::string blue = "#0078bf";
  const std::string green = "#00bf00";
  const std::string yellow = "#ffff00";
  const std::string bright_orange = "#ffc800";
  const std::string orange = "#bf9900";
  const std::string bright_red = "#ff0000";
  const std::string red = "#b62300";
  const std::string cyan = "#00bfbf";
  const std::string magenta = "#b400a3";
  const std::string grey = "#969696";
  const std::string dark_grey = "#444444";
} constants;

#endif // CONSTANTS_HPP
