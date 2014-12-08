#ifndef LOAD_MODULE_HPP
#define LOAD_MODULE_HPP

#include "module.hpp"

#include <utility>
#include <string>

class load_module : public module {

public:
  load_module(const std::string, const std::string);
  virtual ~load_module();

  virtual void update();
  virtual std::pair<std::string, bool> format() const;

private:
  float avg1, avg5, avg15;
  uint nproc;

}; // class load_module

#endif // LOAD_MODULE_HPP
