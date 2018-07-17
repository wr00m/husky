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

  Material(const Vector3f &diffuseColor)
    : name()
    , diffuseColor(diffuseColor)
    , specularColor(1, 1, 1)
    , ambientColor(0.05f, 0.05f, 0.05f)
    , emissiveColor(0, 0, 0)
    , opacity(1)
    , shininess(0)
    , shininessStrength(1)
    , wireframe(false)
    , twoSided(false)
  {
  }

#pragma warning(suppress: 4251)
  std::string name;
  Vector3f diffuseColor;
  Vector3f specularColor;
  Vector3f ambientColor;
  Vector3f emissiveColor;
  float opacity;
  float shininess;
  float shininessStrength;
  bool wireframe;
  bool twoSided;
};

}
