#ifndef MODULES_HPP
#define MODULES_HPP

#include "cpu_module.hpp"
#include "mem_module.hpp"
#include "who_module.hpp"
#include "load_module.hpp"
#include "remote_wrapper.hpp"

#include <string>

class local_modules final {

public:
  local_modules(const int);
  void update();
  std::string format() const;

private:
  cpu_module cpu;
  mem_module mem;
  load_module load;

}; // class local_modules

class remote_modules final {

public:
  remote_modules(const std::string, const int);
  remote_modules(remote_modules&&);
  void update();
  std::string format() const;
  const std::string hostname() const;

private:
  const std::string host;
  remote_wrapper cpu;
  remote_wrapper mem;
  remote_wrapper who;
  remote_wrapper load;

}; // class remote_modules

#endif // MODULES_HPP
