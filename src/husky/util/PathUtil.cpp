#include <husky/util/PathUtil.hpp>
#include <husky/util/StringUtil.hpp>

namespace husky {

std::string PathUtil::getExtension(const std::string &path)
{
  std::string filename = getFilename(path);
  size_t n = filename.find_first_of(".");
  return (n != std::string::npos ? filename.substr(n) : "");
}

std::string PathUtil::getDirectory(const std::string &path)
{
  size_t n = path.find_last_of("/\\");
  if (n == std::string::npos) {
    return "";
  }
  if (n == 2 && path[n - 1] == ':') { // "C:/" or "C:/dir"
    if (path.length() == 3) { // C:/
      return "";
    }
    n += 1; // Keep slash to avoid path ending with ':'
  }
  return path.substr(0, n);
}

std::string PathUtil::getFilename(const std::string &path)
{
  size_t n = path.find_last_of("/\\");
  return (n != std::string::npos ? path.substr(n + 1) : path);
}

std::string PathUtil::getFilenameWithoutExtension(const std::string &path)
{
  return removeExtension(getFilename(path));
}

std::string PathUtil::removeExtension(const std::string &path)
{
  std::string ext = getExtension(path);
  return path.substr(0, path.length() - ext.length());
}

std::string PathUtil::removeTrailingSlash(const std::string &path)
{
  return StringUtil::rtrim(path, "/\\");
}

std::string PathUtil::combine(const std::string &path1, const std::string &path2)
{
  if (path2.empty()) {
    return path1;
  }
  if (path1.empty()) {
    return path2;
  }
  if (path1.back() == '/' || path1.back() == '\\' || path2.front() == '/' || path2.front() == '\\') {
    return path1 + path2;
  }
  return path1 + separator + path2;
}

}
