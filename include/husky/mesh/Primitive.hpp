#pragma once

#include <husky/mesh/SimpleMesh.hpp>

namespace husky {

class HUSKY_DLL Primitive
{
public:
  static SimpleMesh box(double sizeX, double sizeY, double sizeZ);
  static SimpleMesh cylinder(double radius, double height, bool cap = false, int segmentCount = 16);
  static SimpleMesh sphere(double radius, int uSegmentCount = 32, int vSegmentCount = 16);
  static SimpleMesh torus(double innerRadius, double outerRadius, int uSegmentCount = 32, int vSegmentCount = 16);
  //static SimpleMesh capsule();
};

}
