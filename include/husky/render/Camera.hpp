#pragma once

#include <husky/math/Vector3.hpp>

namespace husky {

template<typename T>
class __declspec(dllexport) Camera
{
public:
  Vector3d position;
  Vector3d attitude;

  Camera() : position(), attitude() {}
};

}
