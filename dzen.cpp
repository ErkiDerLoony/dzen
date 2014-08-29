#include <unistd.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <map>
#include <sstream>

using namespace std;

class log {

public:

  typedef basic_ostream<char, char_traits<char>> std_endl;
  friend log& operator<<(log& stream, std_endl& (*f)(std_endl&)) {
#ifdef DEBUG
    wcout << L"\033[30m" << stream.buffer.str() << L"\033[m" << endl;
    stream.buffer.str(L"");
#endif
    return stream;
  }

  template<typename T>
  friend log& operator<<(log& stream, const T& value) {
#ifdef DEBUG
    stream.buffer << value;
#endif
    return stream;
  }

private:
  wstringstream buffer;

};

log debug;

class cpu final {

public:
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

  const long index;
  static long counter;

  cpu() : index(counter++) {
    debug << L"CPU № " << index << " created." << endl;
  }

  virtual ~cpu() {
    debug << L"CPU № " << index << " deleted." << endl;
  }

  cpu(const cpu& other) : index(other.index) {
    debug << L"CPU № " << index << " copied." << endl;
  }

private:
  cpu& operator=(const cpu& other);

};

long cpu::counter = 0;

istream& operator>>(istream& stream, cpu& cpu) {
  stream >> cpu.user >> cpu.nice >> cpu.system >> cpu.idle >> cpu.iowait >> cpu.irq >> cpu.softirq >> cpu.steal >> cpu.guest >> cpu.guest_nice;
  return stream;
}

ostream& operator<<(ostream& stream, const cpu& cpu) {
  stream << "(user: " << cpu.user
         << ", nice: " << cpu.nice
         << ", system: " << cpu.system
         << ", idle: " << cpu.idle
         << ", iowait: " << cpu.iowait
         << ", irq: " << cpu.irq
         << ", softirq: " << cpu.softirq
         << ", steal: " << cpu.steal
         << ", guest: " << cpu.guest
         << ", guest_nice: " << cpu.guest_nice << ")";
  return stream;
}

cpu operator-(const cpu& first, const cpu& second) {
  cpu result;
  result.user = first.user - second.user;
  return result;
}

class proc_stat final {

public:
  cpu sum;
  map<string, cpu> cpus;

  const long index;
  static long counter;

  proc_stat(cpu sum, map<string, cpu> cpus) : sum(sum), cpus(cpus), index(counter++) {
    debug << L"Stat № " << index << L" created." << endl;
  }

  proc_stat(const proc_stat& other) : sum(other.sum), cpus(other.cpus), index(other.index) {
    debug << L"Stat № " << index << L" copied." << endl;
  }

  virtual ~proc_stat() {
    debug << L"Stat  № " << index << L" deleted." << endl;
  }

private:
  proc_stat& operator=(const proc_stat& other);
};

long proc_stat::counter = 0;

proc_stat read_proc_stat(string filename) {
  ifstream in(filename);

  string line;
  map<string, cpu> cpus;
  cpu sum;

  while (getline(in, line)) {

    if (line.substr(0, 4) == "cpu ") {
      stringstream s(line);
      string name;
      s >> name >> sum;
    } else if (line.substr(0, 3) == "cpu") {
      stringstream s(line);
      string name;
      cpu c;
      s >> name >> c;
      cpus.insert(make_pair(name, c));
    } else {
      break;
    }
  }

  return proc_stat(sum, cpus);
}

int main(int argc, char** argv) {
  // Fix io of UTF-8 characters.
  std::ios_base::sync_with_stdio(false);
  std::wcout.imbue(std::locale(""));
  proc_stat before(read_proc_stat("/proc/stat"));
  // sleep(1);
  // proc_stat after(read_proc_stat("/proc/stat"));
  // cpu diff = after.sum - before.sum;
  // cout << before.sum.user << " - " << after.sum.user << " = " << diff.user << endl;
  return EXIT_SUCCESS;
}
