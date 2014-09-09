#ifndef MODULES_HPP
#define MODULES_HPP

#include "cpu_module.hpp"
#include "remote_wrapper.hpp"

#include <string>

class local_modules final {

public:
  local_modules();
  void update();
  std::string format() const;

private:
  cpu_module cpu;

}; // class local_modules

class remote_modules final {

public:
  remote_modules(const std::string);
  remote_modules(remote_modules&&);
  void update();
  std::string format() const;
  const std::string hostname() const;

private:
  const std::string host;
  remote_wrapper cpu;

}; // class remote_modules

#endif // MODULES_HPP
