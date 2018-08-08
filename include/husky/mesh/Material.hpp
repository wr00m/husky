#pragma once

#include <husky/math/Vector3.hpp>
#include <stdint.h>
#include <string>

namespace husky
{

class HUSKY_DLL Material
{
public:
  Material()
    : Material(Vector3f(1, 1, 1))
  {
  }

  Material(const Vector3f &diffuse, unsigned int textureHandle = 0)
    : name()
    , diffuse(diffuse)
    , specular(1.f, 1.f, 1.f)
    , ambient(0.05f, 0.05f, 0.05f)
    , emissive(0.f, 0.f, 0.f)
    , opacity(1.f)
    , shininess(100.f)
    , shininessStrength(1.f)
    , lineWidth(2.f)
    , wireframe(false)
    , twoSided(false)
    , depthTest(true)
    , textureHandle(textureHandle)
  {
  }

  std::string name;
  Vector3f diffuse;
  Vector3f specular;
  Vector3f ambient;
  Vector3f emissive;
  float opacity;
  float shininess;
  float shininessStrength;
  float lineWidth;
  bool wireframe;
  bool twoSided;
  bool depthTest;
  unsigned int textureHandle;
};

}
