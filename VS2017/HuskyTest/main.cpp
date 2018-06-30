#include <cstdio>
#include <Husky/Math/Matrix44.hpp>

int main()
{
  husky::Vector3d a(0, 1, 2);
  husky::Vector3d b(4, 4, 4);
  double c = a.dot(b);
  auto d = -a;
  printf("%f\n", c);
  return 0;
}
