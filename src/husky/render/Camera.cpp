#include <husky/render/Camera.hpp>

namespace husky {

Camera::Camera()
  : position()
  , attitude()
{
}

Camera::Camera(const Vector3d &position, const Quaterniond &attitude)
  : position(position)
  , attitude(attitude)
{
}

Vector3d Camera::right() const
{
  return attitude * Vector3d(1, 0, 0);
}

Vector3d Camera::forward() const
{
  return attitude * Vector3d(0, 1, 0);
}

Vector3d Camera::up() const
{
  return attitude * Vector3d(0, 0, 1);
}

Frustum Camera::frustum() const
{
  return Frustum(projection, view);
}

void Camera::buildViewMatrix()
{
  view = Matrix44d::lookAt(position, position + forward(), up());
}

}
