#include <husky/util/StringUtil.hpp>
#include <algorithm>

namespace husky {

std::string StringUtil::trim(const std::string &s, const std::string &trimChars)
{
  return ltrim(rtrim(s, trimChars), trimChars);
}

std::string StringUtil::ltrim(const std::string &s, const std::string &trimChars)
{
  size_t n = s.find_first_not_of(trimChars);
  return (n != std::string::npos ? s.substr(n) : s);
}

std::string StringUtil::rtrim(const std::string &s, const std::string &trimChars)
{
  size_t n = s.find_last_not_of(trimChars);
  return (n != std::string::npos ? s.substr(0, n + 1) : s);
}

std::string StringUtil::toLower(const std::string &s)
{
  std::string r = s; // Copy
  std::transform(r.begin(), r.end(), r.begin(), ::tolower);
  return r;
}

std::string StringUtil::toUpper(const std::string &s)
{
  std::string r = s; // Copy
  std::transform(r.begin(), r.end(), r.begin(), ::toupper);
  return r;
}

bool StringUtil::startsWith(const std::string &s, const std::string &prefix)
{
  if (s.length() < prefix.length()) {
    return false;
  }
  else {
    return s.compare(0, prefix.length(), prefix) == 0;
  }
}

bool StringUtil::endsWith(const std::string &s, const std::string &suffix)
{
  if (s.length() < suffix.length()) {
    return false;
  }
  else {
    return s.compare(s.length() - suffix.length(), suffix.length(), suffix) == 0;
  }
}

}
