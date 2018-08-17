#pragma once

#include <husky/Common.hpp>
#include <cassert>
#include <string>

namespace husky {

enum class ImageFormat
{
  UNDEFINED,
  RGB8,
  RGBA8,
};

class HUSKY_DLL Image
{
public:
  static Image load(const std::string &filePath);

  Image();
  Image(const Image &other) = delete;
  Image(Image &&other);
  Image(int width, int height, ImageFormat format);
  virtual ~Image();

  bool save(const std::string &filePath) const;
  bool valid() const;
  const std::uint8_t* data() const;
  std::uint8_t* data();

  template<typename T>
  void setPixel(int x, int y, const T &px)
  {
    assert(sizeof(T) == numBytesPerPixel);
    const std::uint8_t *b = reinterpret_cast<const std::uint8_t*>(&px);
    const size_t pxIndex = (y * size_t(width)) + x;
    const size_t byteStartIndex = (pxIndex * numBytesPerPixel);
    memcpy(bytes + byteStartIndex, b, sizeof(T));
  }

  const int width, height, numBytesPerPixel;
  const ImageFormat format;
  const size_t numBytesTotal;

private:
  Image(int width, int height, ImageFormat format, std::uint8_t *stbBytes);

  std::uint8_t *bytes;
};

}
