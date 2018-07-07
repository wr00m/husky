#pragma once

#include <husky/mesh/SimpleMesh.hpp>

namespace husky {

class __declspec(dllexport) Primitive
{
public:
  static SimpleMesh uvSphere(double radius, int uSegmentCount = 32, int vSegmentCount = 16);
  static SimpleMesh cylinder(double radius, double height, bool cap = false, int segmentCount = 16);
  static SimpleMesh box(double sizeX, double sizeY, double sizeZ);
  //static SimpleMesh capsule();
};

}
