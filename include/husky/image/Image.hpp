#pragma once

#include <husky/Common.hpp>
#include <vector>

namespace husky {

class Image
{
public:
  HUSKY_DLL Image(int width, int height, int bytesPerPixel);

  template<typename T>
  void setPixel(int x, int y, const T &px)
  {
    const std::uint8_t *b = reinterpret_cast<const std::uint8_t*>(&px);
    const int pxIndex = (y * width) + x;
    const int byteStartIndex = pxIndex * bytesPerPixel;
    std::copy(b, b + sizeof(T), bytes.begin() + byteStartIndex);
  }

  int width, height, bytesPerPixel;
  std::vector<uint8_t> bytes;
};

}
