#ifndef LOG_HPP
#define LOG_HPP

#include <sstream>
#include <ostream>

class log final {

public:

  template<typename T>
  log& operator<<(const T& text) {
#ifdef DEBUG
    buffer << text;
#endif
    return *this;
  }

  typedef std::basic_ostream<char, std::char_traits<char>> endl_t;
  log& operator<<(endl_t&(*endl)(endl_t&)) {
#ifdef DEBUG
    std::clog << "\033[30m" << buffer.str() << "\033[m" << endl;
    buffer.str("");
    buffer.clear();
#endif
    return *this;
  }

private:
  std::stringstream buffer;

}; // class log

log debug;

#endif // LOG_HPP
