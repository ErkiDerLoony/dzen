#ifndef WHO_MODULE_HPP
#define WHO_MODULE_HPP

#include "module.hpp"

#include <string>
#include <set>
#include <map>

class who_module : public module {

public:
  who_module(const std::string);
  virtual ~who_module();

  virtual void update();
  virtual std::string format() const;

private:
  std::set<std::string> names;
  static std::map<std::string, uint> lengths;

  const uint max_length() const;

}; // class who_module

#endif // WHO_MODULE_HPP
