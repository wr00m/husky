#pragma once

#include <husky/image/Image.hpp>

namespace husky {

enum class TextureWrap
{
  REPEAT,
  CLAMP,
};

enum class TextureSampling
{
  NEAREST,
  LINEAR,
};

class HUSKY_DLL Texture
{
public:
  static const Texture& white1x1();

  Texture();
  Texture(const Image &image, TextureWrap wrap = TextureWrap::REPEAT, TextureSampling sampling = TextureSampling::LINEAR, bool mipmaps = true);
  Texture(const std::string &imageFilePath, TextureWrap wrap = TextureWrap::REPEAT, TextureSampling sampling = TextureSampling::LINEAR, bool mipmaps = true);

  bool valid() const;

  unsigned int handle;
  std::string imageFilePath;
  TextureWrap wrap;
  TextureSampling sampling;
  bool mipmaps;
};

}
