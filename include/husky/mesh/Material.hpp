#pragma once

#include <husky/math/Vector3.hpp>
#include <stdint.h>
#include <string>

namespace husky
{

class HUSKY_DLL Material
{
public:
  Material();
  Material(const Vector3f &diffuse);
  Material(const Vector3f &diffuse, unsigned int textureHandle);

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
