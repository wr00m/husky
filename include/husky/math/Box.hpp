#pragma once

#include <vector>
#include <husky/math/Vector3.hpp>

namespace husky
{

class HUSKY_DLL Box
{
public:
  Box();
  Box(const Vector3d &min, const Vector3d &max);

  void init(const Vector3d &pt);
  void expand(const Vector3d &pt);
  Vector3d size() const;
  Vector3d center() const;
  double volume() const;
  std::vector<Vector3d> corners() const;

  Vector3d min, max;
  bool initialized;
};

}
