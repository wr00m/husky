#pragma once

#include <husky/math/Quaternion.hpp>
#include <husky/math/Matrix44.hpp>

namespace husky {

class HUSKY_DLL Camera
{
public:
  Vector3d position;
  Quaterniond attitude;

  Camera();
  Camera(const Vector3d &position, const Quaterniond &attitude);

  Vector3d right() const;
  Vector3d forward() const;
  Vector3d up() const;
  Matrix44d getViewMatrix() const;
};

}
