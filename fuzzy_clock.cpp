#include "fuzzy_clock.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

fuzzy_clock::fuzzy_clock() : module(std::vector<std::string>()) {
}

fuzzy_clock::~fuzzy_clock() {
}

void fuzzy_clock::update() {
}

std::string fuzzy_clock::format(const int hour) const {
  switch (hour) {
  case 0:
    return "Mitternacht";
  case 1:
  case 13:
    return "Eins";
  case 2:
  case 14:
    return "Zwei";
  case 3:
  case 15:
    return "Drei";
  case 4:
  case 16:
    return "Vier";
  case 5:
  case 17:
    return "Fünf";
  case 6:
  case 18:
    return "Sechs";
  case 7:
  case 19:
    return "Sieben";
  case 8:
  case 20:
    return "Acht";
  case 9:
  case 21:
    return "Neun";
  case 10:
  case 22:
    return "Zehn";
  case 11:
  case 23:
    return "Elf";
  case 12:
    return "Zwölf";
  default:
    std::stringstream ss;
    ss << "Invalid hour: " << hour;
    throw std::runtime_error(ss.str());
  }
}

std::string fuzzy_clock::format(const int hour, const int min, const int sec) const {
  auto f = min + sec / 60.0;
  std::stringstream ss;

#ifdef DEBUG
  std::cerr << "\033[30mf = " << f << "\033[0m" << std::endl;
#endif

  if (f < 1) {

#ifdef DEBUG
    std::cerr << "\033[30mMinute factor < 1\033[0m" << std::endl;
#endif

    if (hour == 0){
      return "Mitternacht";
    } else if (hour == 1) {
      return "Ein Uhr";
    } else {
      ss << format(hour) << " Uhr";
    }

  } else if (f > 59) {

#ifdef DEBUG
    std::cerr << "\033[30mMinute factor > 59\033[0m" << std::endl;
#endif

    if (hour == 23) {
      return "Mitternacht";
    } else if (hour == 0) {
      return "Ein Uhr";
    } else {
      ss << format((hour + 1) % 24) << " Uhr";
    }

  } else {

    if (f < 2.5) {
      ss << "Kurz nach ";
    } else if (f < 7.5) {
      ss << "Fünf nach ";
    } else if (f < 12.5) {
      ss << "Zehn nach ";
    } else if (f < 17.5) {
      ss << "Viertel nach ";
    } else if (f < 22.5) {
      ss << "Zwanzig nach ";
    } else if (f < 27.5) {
      ss << "Kurz vor halb ";
    } else if (f < 32.5) {
      ss << "Halb ";
    } else if (f < 37.5) {
      ss << "Kurz nach halb ";
    } else if (f < 42.5) {
      ss << "Zwanzig vor ";
    } else if (f < 47.5) {
      ss << "Viertel vor ";
    } else if (f < 52.5) {
      ss << "Zehn vor ";
    } else if (f < 57.5) {
      ss << "Fünf vor ";
    } else {
      ss << "Kurz vor ";
    }

    if (hour == 23 && f < 57.5 && f >= 22.5) {
      ss << "Zwölf";
    } else if (hour == 0 && f >= 2.5) {
      ss << "Zwölf";
    } else {

      if (f < 22.5) {
        ss << format(hour);
      } else {
        ss << format((hour + 1) % 24);
      }
    }
  }

  return ss.str();
}

std::pair<std::string, bool> fuzzy_clock::format() const {
  time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  tm local = *localtime(&tt);
  return make_pair(format(local.tm_hour, local.tm_min, local.tm_sec), false);
}
