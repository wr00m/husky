#pragma once

#include <husky/math/Vector3.hpp>
#include <vector>

namespace husky
{

class Box;

class HUSKY_DLL Sphere
{
public:
  Sphere();
  Sphere(const Vector3d &center, double radius);
  Sphere(const Vector3d &center, const std::vector<Vector3d> &pts);
  Sphere(const std::vector<Vector3d> &pts);

  void init(const Vector3d &pt);
  void expand(const Vector3d &pt);
  void expand(const std::vector<Vector3d> &pts);
  void expand(const Box &box);
  void expand(const Sphere &sphere);
  Vector3d min() const;
  Vector3d max() const;
  double volume() const;

  Vector3d center;
  double radius;
  bool initialized;
};

}
