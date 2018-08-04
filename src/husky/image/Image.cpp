#include <husky/image/Image.hpp>
#include <husky/Log.hpp>
#include <husky/util/StringUtil.hpp>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "stb_image_write.h"

namespace fs = std::experimental::filesystem;

namespace husky {

Image Image::load(const std::string &filePath)
{
  int width, height, bytesPerPixel;
  if (stbi_uc *bytes = stbi_load(filePath.c_str(), &width, &height, &bytesPerPixel, STBI_default)) {
    if (bytesPerPixel == 3) {
      return { width, height, ImageFormat::RGB8, bytes };
    }
    else if (bytesPerPixel == 4) {
      return { width, height, ImageFormat::RGBA8, bytes };
    }
    else {
      return {}; // Invalid
    }
  }
  else {
    Log::warning("Failed to load image: %s", filePath.c_str());
    return {}; // Invalid
  }
}

Image::Image()
  : Image(0, 0, ImageFormat::UNDEFINED) // Invalid
{
}

Image::Image(Image &&other)
  : Image(other.width, other.height, other.format, other.bytes)
{
  other.bytes = nullptr; // Assume ownership of byte array
}

Image::Image(int width, int height, ImageFormat format)
  : Image(width, height, format, nullptr)
{
}

static constexpr int getBytesPerPixel(ImageFormat format)
{
  switch (format) {
  case ImageFormat::RGB8      : return 3;
  case ImageFormat::RGBA8     : return 4;
  case ImageFormat::UNDEFINED : return 0;
  default                     : return 0;
  }
}

Image::Image(int width, int height, ImageFormat format, std::uint8_t *stbBytes)
  : width(width)
  , height(height)
  , format(format)
  , numBytesPerPixel(getBytesPerPixel(format))
  , numBytesTotal(size_t(width) * size_t(height) * numBytesPerPixel)
  , bytes(stbBytes) // Assume ownership of byte array
{
  if (bytes == nullptr) {
    bytes = static_cast<std::uint8_t*>(stbi__malloc(numBytesTotal)); // Allocate byte array
  }
}

bool Image::save(const std::string &filePath) const
{
  const auto f = fs::u8path(filePath);
  const auto p = f.parent_path();
  if (!fs::is_directory(p) && !fs::create_directories(p)) {
    Log::warning("Failed to create directory: %s", p.string().c_str());
    return false;
  }

  const std::string ext = StringUtil::toLower(f.extension().string());

  bool ok = true;

  if (ext == ".png") {
    ok = stbi_write_png(filePath.c_str(), width, height, numBytesPerPixel, bytes, 0);
  }
  else if (ext == ".jpg" || ext == ".jpeg") {
    ok = stbi_write_jpg(filePath.c_str(), width, height, numBytesPerPixel, bytes, 90);
  }
  else if (ext == ".bmp") {
    ok = stbi_write_bmp(filePath.c_str(), width, height, numBytesPerPixel, bytes);
  }
  else if (ext == ".tga") {
    ok = stbi_write_tga(filePath.c_str(), width, height, numBytesPerPixel, bytes);
  }
  else {
    Log::warning("Unsupported image file extension: %s", ext.c_str());
  }

  if (!ok) {
    Log::warning("Failed to save image: %s", filePath.c_str());
    return false;
  }

  return true;
}

bool Image::valid() const
{
  return (width > 0 && height > 0 && format != ImageFormat::UNDEFINED && numBytesPerPixel > 0 && numBytesTotal > 0 && bytes != nullptr);
}

const std::uint8_t* Image::data() const
{
  return bytes;
}

Image::~Image()
{
  if (bytes != nullptr) {
    stbi_image_free(bytes);
    bytes = nullptr;
  }
}

}
