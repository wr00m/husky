#pragma once

#include <husky/math/Matrix44.hpp>
#include <string>

namespace husky {

class HUSKY_DLL Bone
{
public:
  Bone(const std::string &name);
  Bone(const std::string &name, int parent);
  Bone(const std::string &name, int parent, const Matrix44f &mtx);

  std::string name;
  //int index;
  int parent;
  Matrix44f mtx;
  Matrix44f mtxCombined;
};

}
