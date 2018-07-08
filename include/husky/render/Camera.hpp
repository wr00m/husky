#pragma once

#include <husky/math/Quaternion.hpp>

namespace husky {

class HUSKY_DLL Camera
{
public:
  Vector3d position;
  Quaterniond attitude;

  Camera();
};

}
