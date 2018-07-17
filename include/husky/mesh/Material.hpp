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
    : name()
    , diffuseColor(255, 255, 255)
    , specularColor(255, 255, 255)
    , ambientColor(255, 255, 255)
    , emissiveColor(0, 0, 0)
    , opacity(255)
    , shininess(0)
    , shininessStrength(255)
    , wireframe(false)
    , twoSided(false)
  {
  }

#pragma warning(suppress: 4251)
  std::string name;
  Vector3b diffuseColor;
  Vector3b specularColor;
  Vector3b ambientColor;
  Vector3b emissiveColor;
  std::uint8_t opacity;
  std::uint8_t shininess;
  std::uint8_t shininessStrength;
  bool wireframe;
  bool twoSided;
};

}
