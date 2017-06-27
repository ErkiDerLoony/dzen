#include "../fuzzy_clock.hpp"

#include <iostream>

using namespace std;

void test(int hour, int min, int sec, std::string target) {
  fuzzy_clock clock;

#ifdef DEBUG
  std::cerr << "\033[30mtest(" << hour << ", " << min << ", " << sec << ")" << std::endl;
#endif

  std::string result = clock.format(hour, min, sec);

  if (result != target) {
    std::cerr << "\033[31;1mFAILURE: format(" << hour << ", " << min << ", " << sec << ") should be »" << target << "« but was »" << result << "«!\033[0m" << std::endl;
    exit(1);
  }
}

int main(int argc, char** argv) {
  test(22, 47, 29, "Viertel vor Elf");
  test(22, 47, 30, "Zehn vor Elf");
  test(22, 47, 31, "Zehn vor Elf");
  test(22, 53, 0, "Fünf vor Elf");
  test(22, 58, 0, "Kurz vor Elf");
  test(22, 58, 59, "Kurz vor Elf");
  test(22, 59, 0, "Kurz vor Elf");
  test(22, 59, 1, "Elf Uhr");
  test(22, 59, 59, "Elf Uhr");
  test(23, 0, 0, "Elf Uhr");
  test(23, 0, 59, "Elf Uhr");
  test(23, 1, 0, "Kurz nach Elf");
  test(23, 22, 29, "Zwanzig nach Elf");
  test(23, 22, 30, "Kurz vor halb Zwölf");
  test(23, 30, 0, "Halb Zwölf");
  test(23, 32, 31, "Kurz nach halb Zwölf");
  test(23, 38, 0, "Zwanzig vor Zwölf");
  test(23, 43, 0, "Viertel vor Zwölf");
  test(23, 48, 0, "Zehn vor Zwölf");
  test(23, 53, 0, "Fünf vor Zwölf");
  test(23, 58, 0, "Kurz vor Mitternacht");
  test(23, 59, 59, "Mitternacht");
  test(0, 0, 0, "Mitternacht");
  test(0, 0, 1, "Mitternacht");
  test(0, 0, 59, "Mitternacht");
  test(0, 1, 0, "Kurz nach Mitternacht");
  test(0, 2, 31, "Fünf nach Zwölf");
  test(0, 8, 0, "Zehn nach Zwölf");
  test(0, 13, 0, "Viertel nach Zwölf");

  test(12, 0, 0, "Zwölf Uhr");
  test(1, 0, 0, "Ein Uhr");
  test(1, 5, 0, "Fünf nach Eins");
}
