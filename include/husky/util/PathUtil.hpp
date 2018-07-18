#pragma once

#include <husky/Common.hpp>
#include <string>

namespace husky {

class HUSKY_DLL PathUtil
{
public:
#if defined(HUSKY_WINDOWS)
  static constexpr char separator = '\\';
#else
  static constexpr char separator = '/';
#endif

  static std::string getExtension(const std::string &path);
  static std::string getDirectory(const std::string &path);
  static std::string getFilename(const std::string &path);
  static std::string getFilenameWithoutExtension(const std::string &path);
  static std::string removeExtension(const std::string &path);
  static std::string removeTrailingSlash(const std::string &path);
  static std::string combine(const std::string &path1, const std::string &path2);
};

}
