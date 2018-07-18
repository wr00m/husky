#pragma once

#include <husky/Common.hpp>
#include <string>

namespace husky {

class HUSKY_DLL Image
{
public:
  static Image load(const std::string &filePath);

  Image();
  Image(int width, int height, int bytesPerPixel);
  virtual ~Image();

  bool valid() const;
  const std::uint8_t* data() const;

  template<typename T>
  void setPixel(int x, int y, const T &px)
  {
    const std::uint8_t *b = reinterpret_cast<const std::uint8_t*>(&px);
    const size_t pxIndex = (y * size_t(width)) + x;
    const size_t byteStartIndex = (pxIndex * bytesPerPixel);
    memcpy(bytes + byteStartIndex, b, sizeof(T));
  }

  const int width, height, bytesPerPixel;
  const size_t totalByteCount;

private:
  Image(int width, int height, int bytesPerPixel, std::uint8_t *stbBytes);

  std::uint8_t *bytes;
};

}
