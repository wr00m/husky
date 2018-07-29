#include <husky/util/SharedResource.hpp>
#include <husky/Log.hpp>
#include <fstream>

namespace husky {

std::map<std::string, std::shared_ptr<const std::vector<std::uint8_t>>> SharedResource::bytesMap;
std::map<std::string, std::shared_ptr<const Image>> SharedResource::imageMap;

std::shared_ptr<const std::vector<std::uint8_t>> SharedResource::loadBytes(const std::string &filePath)
{
  const auto it = bytesMap.find(filePath);
  if (it != bytesMap.end()) {
    return it->second;
  }

  std::ifstream ifs(filePath, std::ios::binary | std::ios::in | std::ios::ate); // Seek to end
  if (ifs.fail()) {
    Log::warning("Unable to load file: %s", filePath.c_str());
    return nullptr;
  }

  std::ifstream::pos_type len = ifs.tellg(); // Determine file size
  auto bytes = std::make_shared<std::vector<std::uint8_t>>(len); // Allocate byte vector
  ifs.seekg(0, std::ios::beg); // Seek to beginning
  ifs.read((char*)bytes->data(), len);

  bytesMap[filePath] = bytes;
  return bytes;
}

std::shared_ptr<const Image> SharedResource::loadImage(const std::string &filePath)
{
  auto image = std::make_shared<Image>(Image::load(filePath));

  if (image == nullptr) {
    Log::warning("Unable to load image: %s", filePath.c_str());
    return nullptr;
  }

  imageMap[filePath] = image;
  return image;
}

}
