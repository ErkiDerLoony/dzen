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

#ifndef COMMAND_LINE_PARSER_HPP
#define COMMAND_LINE_PARSER_HPP

#include <map>
#include <vector>
#include <iostream>
#include <experimental/optional>

class command_line_parser {

public:
  command_line_parser(int argc, char** argv);
  virtual ~command_line_parser();

  bool has(std::initializer_list<std::string> keys) const;

  template<typename T>
  bool has(T begin, T end) const;

  template<typename T>
  bool has(T iterable) const;

  std::experimental::optional<std::string> pop(std::initializer_list<std::string> keys);

  template<typename T>
  std::experimental::optional<std::string> pop(T iterable);

  template<typename T>
  std::experimental::optional<std::string> pop(T begin, T end);

  /**
   * Issue a warning to stderr for each argument still remaining in this parser.
   */
  void warn() const;

private:
  std::vector<std::string> list;
  std::map<std::string, std::experimental::optional<std::string>> args;

  friend std::ostream& operator<<(std::ostream&, const command_line_parser&);

}; // class command_line_parser

template<typename T>
bool command_line_parser::has(T iterable) const {
  return has(iterable.begin(), iterable.end());
}

template<typename T>
bool command_line_parser::has(T begin, T end) const {

  for (auto it = begin; it != end; it++) {
    auto key = *it;

    while (key.substr(0, 1) == "-") {
      key = key.substr(1);
    }

    auto value = args.find(key);

    if (value != args.end()) {
      return true;
    }
  }

  return false;
}

template<typename T>
std::experimental::optional<std::string> command_line_parser::pop(T begin, T end) {
  std::experimental::optional<std::string> result;
  std::vector<std::string> to_remove;

  for (auto it = begin; it != end; it++) {
    auto key = *it;

    while (key.substr(0, 1) == "-") {
      key = key.substr(1);
    }

    auto found = args.find(key);

    if (found != args.end() && !result && (*found).second) {
      result.emplace((*found).second.value());
    }

    if (found != args.end()) {
      to_remove.push_back(key);
    }
  }

  for (std::string key : to_remove) {
    args.erase(key);
  }

  return result;
}

template<typename T>
std::experimental::optional<std::string> command_line_parser::pop(T iterable) {
  return pop(iterable.begin(), iterable.end());
}

#endif // COMMAND_LINE_PARSER_HPP
