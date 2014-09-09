#ifndef CPU_MODULE
#define CPU_MODULE

#include "module.hpp"

#include <string>
#include <vector>
#include <iostream>

class cpu_module : public module {

public:
  cpu_module(const std::string);
  virtual ~cpu_module();
  virtual void update();
  virtual std::string format() const;

private:

  struct cpu {
    long user;
    long nice;
    long system;
    long idle;
    long iowait;
    long irq;
    long softirq;
    long steal;
    long guest;
    long guest_nice;
  };

  struct cpu_info {
    cpu total;
    std::vector<cpu> cpus;

    cpu_info(const cpu, const std::vector<cpu>);
    cpu_info();
    void operator=(const cpu_info& other);
  };

  cpu_info previous;
  cpu_info state;

  friend cpu operator-(const cpu&, const cpu&);
  friend cpu_info operator-(const cpu_info&, const cpu_info&);
  friend std::istream& operator>>(std::istream&, cpu_module::cpu&);
  friend std::ostream& operator<<(std::ostream&, const cpu_module::cpu_info&);

}; // class cpu_module

#endif // CPU_MODULE
