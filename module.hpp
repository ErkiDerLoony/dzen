#ifndef MODULE_HPP
#define MODULE_HPP

#include <initializer_list>
#include <vector>
#include <string>
#include <ostream>

class module {

public:

  /**
   * Create a new module.
   *
   * @param filename  Name of the file this module reads. In case of remote
   *                  modules this filename is pre-processed by
   *                  {@link remote_wrapper} and the remote file is transmitted
   *                  to a local file which is then handed to the actual
   *                  underlying module for further processing.
   */
  module(std::string filename);

  /**
   * Create a new module that requires multiple files.
   *
   * @param files  Names of files this module requires. In case of a remote
   *               module those are automatically transmitted by
   *               {@link remote_wrapper} and the suitable local filename is
   *               handed to the actual underlying module for processing.
   */
  module(const std::initializer_list<std::string> files);

  /**
   * Create a new module that requires multiple files.
   *
   * @param files  Names of files this module requires. In case of a remote
   *               module those are automatically transmitted by
   *               {@link remote_wrapper} and the suitable local filename is
   *               handed to the actual underlying module for processing.
   */
  module(const std::vector<std::string> files);

  virtual ~module();

  std::vector<std::string> filename() const;

  virtual void update() = 0;

  /**
   * Format the output of this module suitable for consumption by dzen2.
   *
   * @return  A pair that contains the formatted output as the first item and a
   *          boolean flag that indicates backtracking as the second item. If
   *          the backtracking flag is {@code true} output of all remote modules
   *          is repeated once more. This allows for a better alignment of the
   *          output of remote modules.
   */
  virtual std::pair<std::string, bool> format() const = 0;

protected:
  void print(std::ostream&, const long&, const std::string&) const;

private:
  const std::vector<std::string> files;

};

#endif // MODULE_HPP
