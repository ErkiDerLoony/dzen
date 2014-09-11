#ifndef MEM_MODULE
#define MEM_MODULE

#include "module.hpp"

#include <string>
#include <sstream>

class mem_module : public module {

public:
  mem_module(const std::string, const int);
  virtual ~mem_module();
  virtual void update();
  virtual std::string format() const;

private:

  struct mem_info {
    ulong total;
    ulong free;
    ulong buffers;
    ulong cached;
    ulong swap_total;
    ulong swap_free;
  };

  mem_info state;
  const int width;

  void output(std::stringstream&) const;
  void output_swap(std::stringstream&) const;

}; // class mem_module

#endif // MEM_MODULE
