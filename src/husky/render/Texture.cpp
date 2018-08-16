#include <husky/render/Texture.hpp>
#include <husky/image/Image.hpp>
#include <husky/math/Vector4.hpp>
#include <husky/util/SharedResource.hpp>
#include <husky/Log.hpp>
#include <glad/glad.h>

namespace husky {

const Texture& Texture::white1x1()
{
  static bool initialized = false;
  static Texture tex;

  if (!initialized) {
    initialized = true;

    Image image(1, 1, husky::ImageFormat::RGBA8);
    image.setPixel(0, 0, Vector4b(255, 255, 255, 255));

    tex = Texture(image, TexWrap::REPEAT, TexFilter::NEAREST, TexMipmaps::NONE);
  }

  return tex;
}

Texture::Texture()
  : handle(0)
{
}

Texture::Texture(const Image &image, TexWrap wrap, TexFilter filter, TexMipmaps mipmaps)
  : handle(0)
  , wrap(wrap)
  , filter(filter)
  , mipmaps(mipmaps)
{
  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);
  
  if (wrap == TexWrap::REPEAT) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  else if (wrap == TexWrap::CLAMP) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  else if (wrap == TexWrap::MIRROR) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  }
  else {
    Log::warning("Unsupported texture wrap: %d", wrap);
  }

  if (filter == TexFilter::LINEAR) {
    if (mipmaps == TexMipmaps::NONE) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
  }
  else if (filter == TexFilter::NEAREST) {
    if (mipmaps == TexMipmaps::NONE) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    }
  }
  else {
    Log::warning("Unsupported texture filter: %d", filter);
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  if (image.format == ImageFormat::RGB8) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
  }
  else if (image.format == ImageFormat::RGBA8) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
  }
  else { // Default checkerboard texture
    static constexpr unsigned int checkerboard[4] = { 0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF, };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerboard);
    Log::warning("Unsupported image format: %d", image.format);
  }

  if (mipmaps != TexMipmaps::NONE) {
    if (mipmaps == TexMipmaps::ANISOTROPIC) {
      //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
      //glTextureParameterf(handle, GL_TEXTURE_MAX_ANISOTROPY, 4.0f); // TODO
      Log::warning("Anisotropic texture filtering not implemented");
    }
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const std::string &imageFilePath, TexWrap wrap, TexFilter filter, TexMipmaps mipmaps)
  : Texture(*SharedResource::loadImage(imageFilePath), wrap, filter, mipmaps)
{
  this->imageFilePath = imageFilePath;
}

bool Texture::valid() const
{
  return (handle != 0);
}

}
