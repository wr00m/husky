#pragma once

#include <husky/math/Quaternion.hpp>
#include <husky/math/Frustum.hpp>

namespace husky {

enum class ProjectionMode
{
  ORTHO,
  PERSP,
  PERSP_FARINF,
  PERSP_FARINF_REVZ,
};

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
  ProjectionMode projMode;
  double perspVerticalFovRad;
  double orthoHeight;
  double aspectRatio;
  double nearDist;
  double farDist;
  Matrix44d proj;
  Matrix44d projInv;
  Matrix44d view;
  Matrix44d viewInv;
};

}
