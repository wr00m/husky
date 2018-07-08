#pragma once

#include <husky/Common.hpp>

namespace husky {

class HUSKY_DLL Log
{
public:
  static void debug(const char *format, ...);
  static void info(const char *format, ...);
  static void warning(const char *format, ...);
  static void error(const char *format, ...);
};

}
