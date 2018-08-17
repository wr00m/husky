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
  Texture(ImageFormat imageFormat, int w, int h, const void *data, TexWrap wrap, TexFilter filter, TexMipmaps mipmaps);
  Texture(const Image &image, TexWrap wrap, TexFilter filter, TexMipmaps mipmaps);
  Texture(const std::string &imageFilePath, TexWrap wrap, TexFilter filter, TexMipmaps mipmaps);

  bool valid() const;
  void uploadImageData(const Image &image) const;
  Image downloadImageData() const;

  unsigned int handle;
  std::string imageFilePath;
  TexWrap wrap;
  TexFilter filter;
  TexMipmaps mipmaps;

private:
  void buildMipmaps() const;
};

}
