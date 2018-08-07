#include <husky/math/Box.hpp>
#include <husky/math/Sphere.hpp>
#include <husky/math/Math.hpp>
#include <algorithm>

namespace husky {

Box::Box()
  : initialized(false)
  , min(0, 0, 0)
  , max(0, 0, 0)
{
}

Box::Box(const Vector3d &min, const Vector3d &max)
  : initialized(true)
  , min(min)
  , max(max)
{
}

Box::Box(const std::vector<Vector3d> &pts)
  : initialized(false)
  , min(0, 0, 0)
  , max(0, 0, 0)
{
  for (const Vector3d &pt : pts) {
    expand(pt);
  }
}

void Box::init(const Vector3d &pt)
{
  min = pt;
  max = pt;
  initialized = true;
}

void Box::expand(const Vector3d &pt)
{
  if (!initialized) {
    init(pt);
    return;
  }

  if      (pt.x < min.x) min.x = pt.x;
  else if (pt.x > max.x) max.x = pt.x;

  if      (pt.y < min.y) min.y = pt.y;
  else if (pt.y > max.y) max.y = pt.y;

  if      (pt.z < min.z) min.z = pt.z;
  else if (pt.z > max.z) max.z = pt.z;
}

void Box::expand(const Box &box)
{
  if (!box.initialized) {
    return;
  }

  if (!initialized) {
    *this = box;
  }

  expand(box.min);
  expand(box.max);
}

void Box::expand(const Sphere &sphere)
{
  expand(Box(sphere.min(), sphere.max()));
}

Vector3d Box::size() const
{
  return (max - min);
}

Vector3d Box::center() const
{
  return (min + max) * 0.5;
}

double Box::radius() const
{
  const Vector3d c = center();
  double r2max = 0;
  for (const Vector3d corner : corners()) {
    double r2 = (corner - c).length2();
    if (r2 > r2max) {
      r2max = r2;
    }
  }
  return std::sqrt(r2max);
}

double Box::volume() const
{
  const Vector3d s = size();
  return s.x * s.y * s.z;
}

std::vector<Vector3d> Box::corners() const
{
  return { { max.x, max.y, max.z },
           { max.x, max.y, min.z },
           { max.x, min.y, max.z },
           { max.x, min.y, min.z },
           { min.x, max.y, max.z },
           { min.x, max.y, min.z },
           { min.x, min.y, max.z },
           { min.x, min.y, min.z } };
}

}
