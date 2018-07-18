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

  Material(const Vector3f &diffuse)
    : name()
    , diffuse(diffuse)
    , specular(1, 1, 1)
    , ambient(0.05f, 0.05f, 0.05f)
    , emissive(0, 0, 0)
    , opacity(1)
    , shininess(0)
    , shininessStrength(1)
    , lineWidth(2)
    , wireframe(false)
    , twoSided(false)
  {
  }

#pragma warning(suppress: 4251)
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
};

}
