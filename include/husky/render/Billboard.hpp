#pragma once

#include <husky/render/Shader.hpp>
#include <husky/mesh/Model.hpp>
//#include <husky/math/Vector2.hpp>
//#include <vector>

namespace husky {

enum class BillboardMode
{
  VIEWPLANE_SPHERICAL,
  VIEWPLANE_CYLINDRICAL,
  SPHERICAL,
  CYLINDRICAL,
  FIXED_PX,
};

class HUSKY_DLL Billboard
{
public:
  //static const std::vector<Vector2f> billboardShapeSquareCenter;
  //static const std::vector<Vector2f> billboardShapeSquareCenterBottom;
  static Shader getBillboardShader(BillboardMode mode);
  static Texture getMultidirectionalBillboardTexture(const Model &mdl);
};

}
