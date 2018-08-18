#pragma once

#include <husky/render/Camera.hpp>

namespace husky {

class HUSKY_DLL Ray
{
public:
  Vector3d startPos;
  Vector3d dir;

  Ray();
  Ray(const Vector3d &startPos, const Vector3d &dir);
  
  void transform(const Matrix44d &mtx);
};

HUSKY_DLL Ray operator*(const Matrix44d &mtx, const Ray &r);

class HUSKY_DLL Viewport
{
public:
  int x;
  int y;
  int width;
  int height;
  
  Viewport();
  Viewport(int x, int y, int width, int height);
  
  void      set(int x, int y, int width, int height);
  double    aspectRatio() const;
  Vector3d  project(const Vector3d &worldPos, const Matrix44d &modelView, const Matrix44d &proj) const;
  Vector3d  unproject(const Vector3d &windowPos, const Matrix44d &modelView, const Matrix44d &proj) const;
  Vector3d  unproject(const Vector3d &windowPos, const Matrix44d &mvpInv) const;
  Ray       getPickingRay(const Vector2d &windowPos, const Camera &cam) const;
};

}
