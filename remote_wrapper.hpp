#ifndef REMOTE_WRAPPER_HPP
#define REMOTE_WRAPPER_HPP

#include "module.hpp"

#include <string>
#include <memory>

class remote_wrapper : public module {

public:
  remote_wrapper(const std::string, std::unique_ptr<module>);
  remote_wrapper(remote_wrapper&&);
  virtual ~remote_wrapper();
  virtual void update();
  virtual std::string format() const;

private:
  const std::string hostname;
  std::unique_ptr<module> inner;

}; // class remote_wrapper

#endif // REMOTE_WRAPPER_HPP
