#pragma once

#include <husky/render/Shader.hpp>

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
  static Shader getBillboardShader(BillboardMode mode);
};

}
