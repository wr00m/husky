#include <husky/render/Texture.hpp>
#include <husky/image/Image.hpp>
#include <husky/Log.hpp>
#include <glad/glad.h>

namespace husky {

unsigned int Texture::uploadTexture(const Image &image)
{
  GLuint textureHandle;

  glGenTextures(1, &textureHandle);
  glBindTexture(GL_TEXTURE_2D, textureHandle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  if (image.format == ImageFormat::RGB8) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
  }
  else if (image.format == ImageFormat::RGBA8) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
  }
  else {
    static constexpr unsigned int checkerboard[4] = { 0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF, };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerboard);
    Log::warning("Unsupported image format, using default checkerboard texture");
  }

  //glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  return textureHandle;
}

}
