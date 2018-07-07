#pragma once

#include <husky/math/Matrix44.hpp>

namespace husky {

class Ray
{
public:
  Vector3d startPos;
  Vector3d dir;
};

class __declspec(dllexport) Viewport
{
public:
  int x; // Left
  int y; // Bottom (??)
  int width;
  int height;
  
  Viewport();
  Viewport(int x, int y, int width, int height);
  
  double    aspectRatio() const;
  Vector3d  project(const Vector3d &worldPos, const Matrix44d &modelView, const Matrix44d &proj) const;
  Vector3d  unproject(const Vector3d &windowPos, const Matrix44d &modelView, const Matrix44d &proj) const;
  Ray       getPickingRay(const Vector2d &windowPos, const Matrix44d &modelView, const Matrix44d &proj) const;
};

}
