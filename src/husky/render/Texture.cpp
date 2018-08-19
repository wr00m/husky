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

    Image image(1, 1, ImageFormat::RGBA8);
    image.setPixel(0, 0, Vector4b(255, 255, 255, 255));

    tex = Texture(image, TexWrap::REPEAT, TexFilter::NEAREST, TexMipmaps::NONE);
  }

  return tex;
}

static bool imageFormatToGL(ImageFormat imageFormat, GLint &internalFormat, GLenum &dataFormat, GLenum &dataType)
{
  switch (imageFormat)
  {
  // TODO: Support more values
  case ImageFormat::RGB8  : { internalFormat = GL_RGB;  dataFormat = GL_RGB;  dataType = GL_UNSIGNED_BYTE;  return true; }
  case ImageFormat::RGBA8 : { internalFormat = GL_RGBA; dataFormat = GL_RGBA; dataType = GL_UNSIGNED_BYTE;  return true; }
  default                 : { internalFormat = 0;       dataFormat = 0;       dataType = 0;                 return false; }
  }
}

static ImageFormat imageFormatFromGL(GLint internalFormat)
{
  switch (internalFormat)
  {
    // TODO: Support more values
  case GL_RGB   : { return ImageFormat::RGB8; }
  case GL_RGBA  : { return ImageFormat::RGBA8; }
  default       : { return ImageFormat::UNDEFINED; }
  }
}

Texture::Texture()
  : handle(0)
  , wrap(TexWrap::REPEAT)
  , filter(TexFilter::LINEAR)
  , mipmaps(TexMipmaps::STANDARD)
{
}

Texture::Texture(ImageFormat imageFormat, int w, int h, TexWrap wrap, TexFilter filter, TexMipmaps mipmaps, const void *data)
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

  GLint internalFormat;
  GLenum dataFormat, dataType;
  if (imageFormatToGL(imageFormat, internalFormat, dataFormat, dataType)) {
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, dataFormat, dataType, data);
    buildMipmaps();
  }
  else {
    Log::warning("Unsupported texture image format: %d", imageFormat);
  }
}

Texture::Texture(const Image &image, TexWrap wrap, TexFilter filter, TexMipmaps mipmaps)
  :Texture(image.format, image.width, image.height, wrap, filter, mipmaps, image.data())
{
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

void Texture::uploadImageData(const Image &image) const
{
  if (!valid()) {
    return;
  }

  glBindTexture(GL_TEXTURE_2D, handle);

  GLint internalFormat;
  GLenum dataFormat, dataType;
  if (imageFormatToGL(image.format, internalFormat, dataFormat, dataType)) {
    glTextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width, image.height, dataFormat, dataType, image.data());
    buildMipmaps();
  }
  else {
    Log::warning("Unsupported texture image format: %d", image.format);
  }
}

Image Texture::downloadImageData() const
{
  if (!valid()) {
    return{};
  }

  glBindTexture(GL_TEXTURE_2D, handle);

  GLint internalFormat;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
  ImageFormat imageFormat = imageFormatFromGL(internalFormat);

  if (imageFormat == ImageFormat::UNDEFINED) {
    return{};
  }

  int w, h;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

  Image image(w, h, imageFormat);
  glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_UNSIGNED_BYTE, image.data()); // TODO: Is this call really correct? (Seems to be working...)
  //glReadPixels(0, 0, w, h, internalFormat, GL_UNSIGNED_BYTE, image.data());

  return image;
}

void Texture::buildMipmaps() const
{
  if (!valid() || mipmaps == TexMipmaps::NONE) {
    return;
  }

  glBindTexture(GL_TEXTURE_2D, handle);

  if (mipmaps == TexMipmaps::ANISOTROPIC) {
    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
    //glTextureParameterf(handle, GL_TEXTURE_MAX_ANISOTROPY, 4.0f); // TODO
    Log::warning("Anisotropic texture filtering not implemented");
  }

  glGenerateMipmap(GL_TEXTURE_2D);
}

MultidirTexture::MultidirTexture()
  : tex()
  , numLon()
  , numLat()
{
}

MultidirTexture::MultidirTexture(Texture &&tex, int numLon, int numLat)
  : tex(tex)
  , numLon(numLon)
  , numLat(numLat)
{
}

}
