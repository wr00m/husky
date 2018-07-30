#pragma once

#include <husky/image/Image.hpp>
#include <map>
#include <string>
#include <vector>

namespace husky {

class HUSKY_DLL SharedResource
{
public:
  static std::shared_ptr<const std::vector<std::uint8_t>> loadBytes(const std::string &filePath);
  static std::shared_ptr<const Image> loadImage(const std::string &filePath);
  static void releaseAll();

private:
  static std::map<std::string, std::shared_ptr<const std::vector<std::uint8_t>>> bytesMap;
  static std::map<std::string, std::shared_ptr<const Image>> imageMap;
};

}
