#pragma once

#include <husky/image/Image.hpp>

namespace husky {

class HUSKY_DLL Texture
{
public:
  static unsigned int uploadTexture(const Image &image);
};

}
