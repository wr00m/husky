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
  , vFovRad(60 * Math::deg2rad)
  , aspectRatio(1)
  , nearDist(0.1)
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
  proj = husky::Matrix44d::perspectiveInfRevZ(vFovRad, aspectRatio, nearDist, &projInv);
}

void Camera::buildViewMatrix()
{
  view = Matrix44d::lookAt(pos, pos + forward(), up(), &viewInv);
}

}
