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

Sphere::Sphere(const Vector3d &center, const std::vector<Vector3d> &pts)
  : initialized(true)
  , center(center)
  , radius(0)
{
  expand(pts);
}

Sphere::Sphere(const std::vector<Vector3d> &pts)
  : initialized(false)
  , center(0, 0, 0)
  , radius(0)
{
  if (!pts.empty()) {
    center = Box(pts).center();
    expand(pts);
    initialized = true;
  }
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

  const double r2 = (pt - center).length2();
  if (r2 > (radius * radius)) {
    radius = std::sqrt(r2);
  }
}

void Sphere::expand(const std::vector<Vector3d> &pts)
{
  if (pts.empty()) {
    return;
  }

  const double r2orig = (radius * radius);
  double r2max = r2orig;
  for (const Vector3d &pt : pts) {
    double r2 = (pt - center).length2();
    if (r2 > r2max) {
      r2max = r2;
    }
  }

  if (r2max > r2orig) {
    radius = std::sqrt(r2max);
  }
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
