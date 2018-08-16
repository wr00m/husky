#pragma once

#include <husky/image/Image.hpp>

namespace husky {

enum class TexWrap
{
  REPEAT,
  CLAMP,
  MIRROR,
};

enum class TexFilter
{
  NEAREST,
  LINEAR,
};

enum class TexMipmaps
{
  NONE,
  STANDARD,
  ANISOTROPIC,
};

class HUSKY_DLL Texture
{
public:
  static const Texture& white1x1();

  Texture();
  Texture(const Image &image, TexWrap wrap = TexWrap::REPEAT, TexFilter filter = TexFilter::LINEAR, TexMipmaps mipmaps = TexMipmaps::STANDARD);
  Texture(const std::string &imageFilePath, TexWrap wrap = TexWrap::REPEAT, TexFilter filter = TexFilter::LINEAR, TexMipmaps mipmaps = TexMipmaps::STANDARD);

  bool valid() const;

  unsigned int handle;
  std::string imageFilePath;
  TexWrap wrap;
  TexFilter filter;
  TexMipmaps mipmaps;
};

}
