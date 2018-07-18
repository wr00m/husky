#pragma once

#include <husky/Common.hpp>
#include <string>

namespace husky {

class HUSKY_DLL StringUtil
{
public:
  static std::string trim(const std::string &s, const std::string &trimChars = " \t\n\v\f\r");
  static std::string ltrim(const std::string &s, const std::string &trimChars = " \t\n\v\f\r");
  static std::string rtrim(const std::string &s, const std::string &trimChars = " \t\n\v\f\r");
  static std::string toLower(const std::string &s);
  static std::string toUpper(const std::string &s);
  static bool startsWith(const std::string &s, const std::string &prefix);
  static bool endsWith(const std::string &s, const std::string &suffix);
};

}
