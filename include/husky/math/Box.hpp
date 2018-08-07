#pragma once

#include <husky/math/Vector3.hpp>
#include <vector>

namespace husky
{

class Sphere;

class HUSKY_DLL Box
{
public:
  Box();
  Box(const Vector3d &min, const Vector3d &max);
  Box(const std::vector<Vector3d> &pts);

  void init(const Vector3d &pt);
  void expand(const Vector3d &pt);
  void expand(const Box &box);
  void expand(const Sphere &sphere);
  Vector3d size() const;
  Vector3d center() const;
  double radius() const;
  double volume() const;
  std::vector<Vector3d> corners() const;

  Vector3d min, max;
  bool initialized;
};

}
