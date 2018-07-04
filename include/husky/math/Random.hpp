#pragma once

#include <husky/math/Vector3.hpp>

namespace husky
{

class __declspec(dllexport) Random
{
public:
  Random(int seed = 0);

  void      seed(int seed);
  int       getInt();
  int       getInt(int min, int max);
  double    getDouble();
  double    getDouble(double min, double max);
  void      getSphericalCoordinates(double &theta, double &phi);
  Vector3d  getDirection();

private:
  int state;
};

}
