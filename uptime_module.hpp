#ifndef UPTIME_MODULE_HPP
#define UPTIME_MODULE_HPP

#include "module.hpp"

#include <string>

class uptime_module : public module {

public:
  uptime_module(const std::string, const uint);
  virtual ~uptime_module();

  virtual void update();
  virtual std::string format() const;

private:
  uint days;
  const uint padding;

}; // class uptime_module

#endif // UPTIME_MODULE_HPP
