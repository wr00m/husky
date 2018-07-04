#include <husky/math/Random.hpp>
#include <husky/math/MathUtil.hpp>
#include <cmath>

#define RANDOM_MAX  0x7FFF
#define LCG_A       214013
#define LCG_C       2531011

namespace husky {

Random::Random(int seed)
{
  this->seed(seed);
}

void Random::seed(int seed)
{
  state = seed;
}

int Random::getInt()
{
  state = LCG_A * state + LCG_C;
  return (state >> 16) & RANDOM_MAX;
}

int Random::getInt(int min, int max)
{
  return min + getInt() % (max - min);
}

double Random::getDouble()
{
  return getInt() / (double)RANDOM_MAX;
}

double Random::getDouble(double min, double max)
{
  return min + (max - min) * getDouble();
}

void Random::getSphericalCoordinates(double &theta, double &phi)
{
  const double u = getDouble();
  const double v = getDouble();
  theta = 2.0 * math::pi * u;
  phi = std::acos(2.0 * v - 1.0);
}

Vector3d Random::getDirection()
{
  double theta, phi;
  getSphericalCoordinates(theta, phi);
  return Vector3d(
    std::cos(theta) * std::sin(phi),
    std::sin(theta) * std::sin(phi),
    std::cos(phi));
}

}
