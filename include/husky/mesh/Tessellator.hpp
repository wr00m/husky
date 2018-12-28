#pragma once

#include <husky/math/Vector4.hpp>
#include <vector>

namespace husky {

class Feature;

class HUSKY_DLL Tessellator
{
public:
  static void tessellate(const Feature &feature, std::vector<Vector3d> &outPts, std::vector<Vector3i> &outTris);
  static void constrainedDelaunayTriangulation(const std::vector<Vector4d> &inPts, std::vector<Vector4d> &outPts, std::vector<Vector3i> &outTris);
};

}
