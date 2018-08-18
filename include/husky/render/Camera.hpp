#pragma once

#include <husky/math/Quaternion.hpp>
#include <husky/math/Frustum.hpp>

namespace husky {

enum class ProjectionMode
{
  ORTHO,
  ORTHO_REVZ,
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
  double hfovRad() const;
  bool isOrtho() const;
  bool isRevZ() const;

  Vector3d pos;
  Quaterniond rot;
  ProjectionMode projMode;
  double vfovRad; // Perspective
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
