#pragma once

#include <husky/math/Quaternion.hpp>
#include <husky/math/Frustum.hpp>

namespace husky {

class HUSKY_DLL Camera
{
public:
  Camera();
  Camera(const Vector3d &pos, const Quaterniond &rot);

  Vector3d right() const;
  Vector3d forward() const;
  Vector3d up() const;
  Frustum frustum() const;
  void buildProjMatrix();
  void buildViewMatrix();

  Vector3d pos;
  Quaterniond rot;
  double vFovRad;
  double aspectRatio;
  double nearDist;
  Matrix44d proj;
  Matrix44d projInv;
  Matrix44d view;
  Matrix44d viewInv;
};

}
