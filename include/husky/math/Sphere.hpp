#pragma once

#include <husky/math/Vector3.hpp>

namespace husky
{

class Box;

class HUSKY_DLL Sphere
{
public:
  //static Sphere getSmallestSphere(const std::vector<Vector3d> &pts);

  Sphere();
  Sphere(const Vector3d &center, double radius);

  void init(const Vector3d &pt);
  void expand(const Vector3d &pt);
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
