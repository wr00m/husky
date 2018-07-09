#pragma once

#include <husky/render/Camera.hpp>

namespace husky {

class HUSKY_DLL Viewport
{
public:
  int x;
  int y;
  int width;
  int height;
  
  Viewport();
  Viewport(int x, int y, int width, int height);
  
  double    aspectRatio() const;
  Vector3d  project(const Vector3d &worldPos, const Matrix44d &modelView, const Matrix44d &proj) const;
  Vector3d  unproject(const Vector3d &windowPos, const Matrix44d &modelView, const Matrix44d &proj) const;
  Vector3d  getPickingRayDir(const Vector2d &mousePos, const Camera &cam) const;
};

}
