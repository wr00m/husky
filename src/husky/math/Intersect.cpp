#include <husky/math/Intersect.hpp>
#include <husky/math/Math.hpp>
#include <husky/math/Vector4.hpp>
#include <algorithm>
#include <cmath>

namespace husky {

// Returns 1 on positive intersection; -1 if negative intersection; 0 otherwise
// http://geomalgorithms.com/a06-_intersect-2.html#intersect3D_RayTriangle%28%29
int Intersect::lineIntersectsPlane(
  const Vector3d &linePt,
  const Vector3d &lineDir,
  const Vector3d &planePt,
  const Vector3d &planeNormal,
  double &t,
  double tolerance)
{
  t = 0;

  // If invalid plane normal
  if (planeNormal.x == 0 && planeNormal.y == 0 && planeNormal.z == 0) {
    return 0; // Treat as no intersection
  }

  const Vector3d w0 = linePt - planePt; // Ray start point relative to the first triangle point

  double num = -planeNormal.dot(w0);
  double den = planeNormal.dot(lineDir);

  // If the ray is parallel to the triangle plane
  if (std::abs(den) < tolerance) {
    return 0; // No intersection point (though the ray lies in the plane if num == 0)
  }

  // Get the ray-plane intersection distance
  const double r = num / den;
  const int rayTriDir = (r < 0 ? -1 : 1); // Ray direction relative to the triangle

  return rayTriDir;
}

int Intersect::lineIntersectsTriangle(
  const Vector3d &linePt,
  const Vector3d &lineDir,
  const Vector3d &v0,
  const Vector3d &v1,
  const Vector3d &v2,
  double &t)
{
  const Vector3d u = v1 - v0; // First triangle edge
  const Vector3d v = v2 - v1; // Second triangle edge
  const Vector3d n = u.cross(v); // Triangle normal (no need to normalize)

  const int rayTriDir = lineIntersectsPlane(linePt, lineDir, v0, n, t);
  if (rayTriDir == 0) {
    return 0;
  }

  const double uu = u.dot(u);
  const double uv = u.dot(v);
  const double vv = v.dot(v);
  const Vector3d intersectionPt = linePt + lineDir * t;
  const Vector3d w = intersectionPt - v0;
  const double wu = w.dot(u);
  const double wv = w.dot(v);
  const double div = uv * uv - uu * vv;

  // Compute and test parametric coordinates (a and b)
  const double a = (uv * wv - vv * wu) / div;
  if (a < 0 || a > 1) {
    return 0;
  }

  const double b = (uv * wu - uu * wv) / div;
  if (b < 0 || a + b > 1) {
    return 0;
  }

  return rayTriDir; // The line intersects the triangle!
}

int Intersect::lineIntersectsQuad(
  const Vector3d &linePt,
  const Vector3d &lineDir,
  const Vector3d &v0,
  const Vector3d &v1,
  const Vector3d &v2,
  const Vector3d &v3,
  double &t)
{
  // Split the quad into two triangles and test them individually
  if (int result = lineIntersectsTriangle(linePt, lineDir, v0, v1, v2, t)) {
    return result;
  }
  else {
    return lineIntersectsTriangle(linePt, lineDir, v0, v2, v3, t);
  }
}

bool Intersect::lineIntersectsSphere(const Vector3d &linePt, const Vector3d &lineDir, const Vector3d &sphereCenter, double sphereRadius, double &t0, double &t1)
{
  Vector3d diff = sphereCenter - linePt;
  double tca = diff.dot(lineDir);
  double d2 = diff.dot(diff) - (tca * tca);

  const double radius2 = (sphereRadius * sphereRadius);
  if (d2 > radius2) {
    return false;
  }

  double thc = std::sqrt(radius2 - d2);
  t0 = (tca - thc);
  t1 = (tca + thc);

  if (t0 > t1) {
    std::swap(t0, t1);
  }

  return true;
}

bool Intersect::lineIntersectsBox(const Vector3d &linePt, const Vector3d &lineDir, const Vector3d &boxMin, const Vector3d &boxMax, double &t0, double &t1)
{
  double txn = (boxMin.x - linePt.x) / lineDir.x;
  double txp = (boxMax.x - linePt.x) / lineDir.x;
  double tyn = (boxMin.y - linePt.y) / lineDir.y;
  double typ = (boxMax.y - linePt.y) / lineDir.y;
  double tzn = (boxMin.z - linePt.z) / lineDir.z;
  double tzp = (boxMax.z - linePt.z) / lineDir.z;

  t0 = std::max(std::max(std::min(txn, txp), std::min(tyn, typ)), std::min(tzn, tzp));
  t1 = std::min(std::min(std::max(txn, txp), std::max(tyn, typ)), std::max(tzn, tzp));

  if (t0 > t1) { // No intersection
    return false;
  }

  return true;
}

}
