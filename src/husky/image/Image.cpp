#include <husky/image/Image.hpp>

namespace husky {

Image::Image(int width, int height, int bytesPerPixel)
  : width(width)
  , height(height)
  , bytesPerPixel(bytesPerPixel)
{
  const int totalByteCount = (width * height * bytesPerPixel);
  bytes.resize(totalByteCount, 0);
}

}
