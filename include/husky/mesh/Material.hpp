#pragma once

#include <husky/math/Vector3.hpp>
#include <husky/render/Texture.hpp>
#include <stdint.h>
#include <string>

namespace husky
{

class HUSKY_DLL Material
{
public:
  Material();
  Material(const Vector3f &diffuse);
  Material(const Vector3f &diffuse, const Texture &tex);

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
  Texture tex;
};

}
