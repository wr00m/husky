#pragma once

#include <husky/math/Quaternion.hpp>
#include <husky/math/Frustum.hpp>

namespace husky {

class HUSKY_DLL Camera
{
public:
  Matrix44d projection;
  Matrix44d view;

  Vector3d position;
  Quaterniond attitude;

  Camera();
  Camera(const Vector3d &position, const Quaterniond &attitude);

  Vector3d right() const;
  Vector3d forward() const;
  Vector3d up() const;
  Frustum frustum() const;
  void buildViewMatrix();
};

}
