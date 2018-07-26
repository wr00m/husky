#include <husky/render/Camera.hpp>
#include <husky/math/Math.hpp>

namespace husky {

Camera::Camera()
  : Camera({ 0, 0, 0 }, { 0, 0, 0, 1 })
{
}

Camera::Camera(const Vector3d &pos, const Quaterniond &rot)
  : pos(pos)
  , rot(rot)
  , projMode(ProjectionMode::PERSP_FARINF_REVZ)
  , perspVerticalFovRad(60 * Math::deg2rad)
  , perspAspectRatio(1)
  , orthoLeft(-1)
  , orthoRight(1)
  , orthoBottom(-1)
  , orthoTop(1)
  , nearDist(0.1)
  , farDist(1000)
  , proj()
  , projInv()
  , view()
  , viewInv()
{
}

Vector3d Camera::right() const
{
  return rot * Vector3d(1, 0, 0);
}

Vector3d Camera::forward() const
{
  return rot * Vector3d(0, 1, 0);
}

Vector3d Camera::up() const
{
  return rot * Vector3d(0, 0, 1);
}

Frustum Camera::frustum() const
{
  return Frustum(proj, view);
}

void Camera::buildProjMatrix()
{
  if (projMode == ProjectionMode::ORTHO) {
    proj = Matrix44d::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, nearDist, farDist, &projInv);
  }
  else if (projMode == ProjectionMode::PERSP) {
    proj = Matrix44d::perspective(perspVerticalFovRad, perspAspectRatio, nearDist, farDist, &projInv);
  }
  else if (projMode == ProjectionMode::PERSP_FARINF) {
    proj = Matrix44d::perspectiveInf(perspVerticalFovRad, perspAspectRatio, nearDist, 0, &projInv);
  }
  else if (projMode == ProjectionMode::PERSP_FARINF_REVZ) {
    proj = Matrix44d::perspectiveInfRevZ(perspVerticalFovRad, perspAspectRatio, nearDist, &projInv);
  }
  else {
    proj = Matrix44d::identity(); // This shouldn't happen...
  }
}

void Camera::buildViewMatrix()
{
  view = Matrix44d::lookAt(pos, pos + forward(), up(), &viewInv);
}

}
