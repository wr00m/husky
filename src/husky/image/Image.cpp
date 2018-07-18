#include <husky/image/Image.hpp>
#include <husky/Log.hpp>
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
  stbi_set_flip_vertically_on_load(true); // Match texture coordinates

  int width, height, bytesPerPixel;
  if (stbi_uc *bytes = stbi_load(filePath.c_str(), &width, &height, &bytesPerPixel, STBI_default)) {
    return { width, height, bytesPerPixel, bytes };
  }
  else {
    Log::warning("Failed to load image: %s", filePath.c_str());
    return {}; // Invalid
  }
}

Image::Image()
  : Image(0, 0, 0) // Invalid
{
}

Image::Image(int width, int height, int bytesPerPixel)
  : Image(width, height, bytesPerPixel, nullptr)
{
}

Image::Image(int width, int height, int bytesPerPixel, std::uint8_t *stbBytes)
  : width(width)
  , height(height)
  , bytesPerPixel(bytesPerPixel)
  , totalByteCount(size_t(width) * size_t(height) * size_t(bytesPerPixel))
{
  if (stbBytes != nullptr) {
    bytes = stbBytes; // Assume ownership of byte array
  }
  else {
    bytes = static_cast<std::uint8_t*>(stbi__malloc(totalByteCount)); // Allocate byte array
  }
}

bool Image::save(const std::string &filePath) const
{
  // Note: This causes an exception for JPEG (594x594 RGB)
  //stbi_flip_vertically_on_write(true); // Match texture coordinates

  const auto f = fs::u8path(filePath);
  const auto p = f.parent_path();
  if (!fs::is_directory(p) && !fs::create_directories(p)) {
    Log::warning("Failed to create directory: %s", p.string().c_str());
    return false;
  }

  std::string ext = f.extension().string();
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  bool ok = true;

  if (ext == ".png") {
    ok = stbi_write_png(filePath.c_str(), width, height, bytesPerPixel, bytes, 0);
  }
  else if (ext == ".jpg" || ext == ".jpeg") {
    ok = stbi_write_jpg(filePath.c_str(), width, height, bytesPerPixel, bytes, 90);
  }
  else if (ext == ".bmp") {
    ok = stbi_write_bmp(filePath.c_str(), width, height, bytesPerPixel, bytes);
  }
  else if (ext == ".tga") {
    ok = stbi_write_tga(filePath.c_str(), width, height, bytesPerPixel, bytes);
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
  return (width > 0 && height > 0 && bytesPerPixel > 0 && totalByteCount > 0 && bytes != nullptr);
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
