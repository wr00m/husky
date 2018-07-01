#include <cstdio>
#include <Husky/Math/Matrix44.hpp>

int main()
{
  husky::Vector3d a(0, 1, 2);
  husky::Vector3d b(4, 4, 4);
  double c = a.dot(b);

  husky::Vector3f f1 = husky::Vector3f(-a);
  husky::Vector3f f2 = (husky::Vector3f)a;
  husky::Vector3f f3 = husky::Vector3f(a);

  husky::Vector3d d1 = -f1;
  husky::Vector3d d2 = f1;
  husky::Vector3d d3 = f1;

  husky::Matrix44d md;
  husky::Matrix44f mf = husky::Matrix44f(md);

  printf("%f\n", c);
  return 0;
}
