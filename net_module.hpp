#ifndef NET_MODULE_HPP
#define NET_MODULE_HPP

#include "module.hpp"

#include <string>
#include <set>
#include <map>

class net_module : public module {

public:
  net_module(std::string);
  virtual ~net_module();

  virtual void update();
  virtual std::string format() const;

private:

  struct net {
    ulong bytes;
    ulong packets;
    ulong errs;
    ulong drop;
    ulong fifo;
    ulong frame;
    ulong compressed;
    ulong multicast;

    net();
  };

  struct net_info {
    std::string name;
    net rx;
    net tx;
  };

  std::map<std::string, net_info> previous;
  std::map<std::string, net_info> diffs;

  friend std::istream& operator>>(std::istream&, net_module::net_info&);
  friend net_info operator-(const net_info&, const net_info&);

}; // class net_module

#endif // NET_MODULE_HPP
