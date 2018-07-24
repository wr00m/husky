#pragma once

#include <husky/math/Matrix44.hpp>
#include <string>

namespace husky {

class HUSKY_DLL Bone
{
public:
  Bone(const std::string &name);
  Bone(const std::string &name, int parent);
  Bone(const std::string &name, int parent, const Matrix44d &mtx);

  std::string name;
  //int index;
  int parent;
  Matrix44d mtx;
  Matrix44d mtxCombined;
};

}
