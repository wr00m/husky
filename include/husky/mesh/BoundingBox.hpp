#pragma once

#include <husky/math/Vector3.hpp>

namespace husky
{

class HUSKY_DLL BoundingBox
{
public:
  BoundingBox();

  void init(const Vector3d &pt);
  void expand(const Vector3d &pt);
  Vector3d size() const;
  Vector3d center() const;

  Vector3d min, max;
  bool initialized;
};

class HUSKY_DLL BoundingSphere
{
public:
  // TODO
};

}
