#include <husky/math/Sphere.hpp>
#include <husky/math/Box.hpp>
#include <husky/math/Math.hpp>
#include <algorithm>

namespace husky {

Sphere::Sphere()
  : initialized(false)
  , center(0, 0, 0)
  , radius(0)
{
}

Sphere::Sphere(const Vector3d &center, double radius)
  : initialized(true)
  , center(center)
  , radius(radius)
{
}

void Sphere::init(const Vector3d &pt)
{
  center = pt;
  radius = 0;
  initialized = true;
}

void Sphere::expand(const Vector3d &pt)
{
  if (!initialized) {
    init(pt);
    return;
  }

  const Vector3d diff = pt - center;
  const double r = diff.length();

  if (r <= radius) {
    return; // Already inside
  }

  const double dr = (r - radius) * 0.5;

  center += diff * (dr / r);
  radius += dr;
}

void Sphere::expand(const Box &box)
{
  expand(Sphere(box.center(), box.radius()));
}

void Sphere::expand(const Sphere &sphere)
{
  if (!sphere.initialized) {
    return;
  }

  if (!initialized) {
    *this = sphere;
    return;
  }

  const Vector3d diff = (sphere.center - center);
  const double r = diff.length();

  if (r + sphere.radius <= radius) {
    return; // Already inside
  }

  if (r + radius <= sphere.radius) {
    *this = sphere;
    return;
  }

  const double newRadius = (radius + r + sphere.radius) * 0.5;
  const double ratio = (newRadius - radius) / r;

  center += (diff * ratio);
  radius = newRadius;
}

Vector3d Sphere::min() const
{
  return (center - radius);
}

Vector3d Sphere::max() const
{
  return (center + radius);
}

double Sphere::volume() const
{
  return (4.0 / 3.0) * Math::pi * (radius * radius * radius);
}

}
