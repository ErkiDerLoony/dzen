#ifndef REMOTE_WRAPPER_HPP
#define REMOTE_WRAPPER_HPP

#include "module.hpp"

#include <string>
#include <memory>
#include <utility>
#include <vector>

class remote_wrapper : public module {

public:
  remote_wrapper(const std::vector<std::string>, const std::string, std::unique_ptr<module>);
  remote_wrapper(remote_wrapper&&);
  virtual ~remote_wrapper();
  virtual void update();
  virtual std::pair<std::string, bool> format() const;

private:
  bool ok;
  const std::vector<std::string> remote_commands;
  const std::string hostname;
  std::unique_ptr<module> inner;

}; // class remote_wrapper

#endif // REMOTE_WRAPPER_HPP
