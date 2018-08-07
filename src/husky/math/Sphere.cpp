#include <husky/math/Sphere.hpp>
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

  double radius2 = (pt - center).length2();
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
