#include <husky/image/Image.hpp>
#include <husky/Log.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "stb_image_write.h"

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
