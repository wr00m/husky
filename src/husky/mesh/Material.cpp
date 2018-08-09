#include <husky/mesh/Material.hpp>

namespace husky {

Material::Material()
  : Material(Vector3f(1, 1, 1))
{
}

Material::Material(const Vector3f &diffuse)
  : Material(diffuse, 0)
{
}

Material::Material(const Vector3f &diffuse, unsigned int textureHandle)
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

}
