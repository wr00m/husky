#pragma once

#include <husky/math/Quaternion.hpp>

namespace husky {

class __declspec(dllexport) Camera
{
public:
  Vector3d position;
  Quaterniond attitude;

  Camera();
};

}
