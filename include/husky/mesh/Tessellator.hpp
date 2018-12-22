#pragma once

#include <husky/Common.hpp>
#include <husky/math/Vector3.hpp>
#include <vector>

namespace husky {

class Feature;

class HUSKY_DLL Tessellator
{
public:
  static void tessellate(const Feature &feature, std::vector<Vector3d> &outPts, std::vector<Vector3i> &outTris);
};

}
