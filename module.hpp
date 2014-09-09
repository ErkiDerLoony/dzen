#ifndef MODULE_HPP
#define MODULE_HPP

#include <string>

class module {

public:
  module(const std::string);
  virtual ~module();
  const std::string& filename() const;

  virtual void update() = 0;
  virtual std::string format() const = 0;

private:
  const std::string _filename;

};

#endif // MODULE_HPP
