#include <husky/mesh/BoundingBox.hpp>
#include <algorithm>

namespace husky {

BoundingBox::BoundingBox()
{
}

void BoundingBox::init(const Vector3d &pt)
{
  min = pt;
  max = pt;
}

void BoundingBox::expand(const Vector3d &pt)
{
  if      (pt.x < min.x) min.x = pt.x;
  else if (pt.x > max.x) max.x = pt.x;

  if      (pt.y < min.y) min.y = pt.y;
  else if (pt.y > max.y) max.y = pt.y;

  if      (pt.z < min.z) min.z = pt.z;
  else if (pt.z > max.z) max.z = pt.z;
}

Vector3d BoundingBox::size() const
{
  return (max - min);
}

Vector3d BoundingBox::center() const
{
  return (min + max) * 0.5;
}

}
