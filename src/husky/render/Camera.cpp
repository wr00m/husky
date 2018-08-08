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
  , vfovRad(60 * Math::deg2rad)
  , orthoHeight(20)
  , aspectRatio(1)
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
    double hh = orthoHeight * 0.5;
    double wh = hh * aspectRatio;
    proj = Matrix44d::ortho(-wh, wh, -hh, hh, nearDist, farDist, &projInv);
  }
  else if (projMode == ProjectionMode::PERSP) {
    proj = Matrix44d::perspective(vfovRad, aspectRatio, nearDist, farDist, &projInv);
  }
  else if (projMode == ProjectionMode::PERSP_FARINF) {
    proj = Matrix44d::perspectiveInf(vfovRad, aspectRatio, nearDist, 0, &projInv);
  }
  else if (projMode == ProjectionMode::PERSP_FARINF_REVZ) {
    proj = Matrix44d::perspectiveInfRevZ(vfovRad, aspectRatio, nearDist, &projInv);
  }
  else {
    proj = Matrix44d::identity(); // This shouldn't happen...
  }
}

void Camera::buildViewMatrix()
{
  view = Matrix44d::lookAt(pos, pos + forward(), up(), &viewInv);
}

double Camera::hfovRad() const
{
  double hfovRad = 2.0 * std::atan(std::tan(vfovRad * 0.5) * aspectRatio);
  return hfovRad;
}

}
