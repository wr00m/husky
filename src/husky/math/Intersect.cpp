#include <husky/math/Intersect.hpp>
#include <husky/math/Math.hpp>
#include <husky/math/Vector4.hpp>
#include <vector>
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
  return false; // TODO
}

bool Intersect::lineIntersectsBox(const Vector3d &linePt, const Vector3d &lineDir, const Vector3d &boxMin, const Vector3d &boxMax, double &t0, double &t1)
{
  const Vector3d pts[8] = { // Box corners
    boxMin,                           // ---
    { boxMin.x, boxMax.y, boxMin.z }, // -+-
    { boxMax.x, boxMax.y, boxMin.z }, // ++-
    { boxMax.x, boxMin.y, boxMin.z }, // +--
    { boxMin.x, boxMin.y, boxMax.z }, // --+
    { boxMin.x, boxMax.y, boxMax.z }, // -++
    boxMax,                           // +++
    { boxMax.x, boxMin.y, boxMax.z }, // +-+
  };

  const Vector4i quads[6] = { // Box sides (inconsistent winding order, but that's OK)
    { 3, 2, 6, 7 }, // X+
    { 1, 0, 4, 5 }, // X-
    { 1, 2, 6, 5 }, // Y+
    { 0, 3, 7, 4 }, // Y-
    { 4, 5, 6, 7 }, // Z+
    { 0, 1, 2, 3 }, // Z-
  };

  std::vector<double> ts;
  for (int i = 0; i < 6; i++) {
    const Vector4i &quad = quads[i];
    double t;
    if (lineIntersectsQuad(linePt, lineDir, pts[quad[0]], pts[quad[1]], pts[quad[2]], pts[quad[3]], t)) {
      ts.emplace_back(t);
      if (ts.size() == 2) { // Entry point and exit point found
        if (std::abs(ts[0]) < std::abs(ts[1])) {
          t0 = ts[0];
          t1 = ts[1];
          return true;
        }
        else {
          t0 = ts[1];
          t1 = ts[0];
          return true;
        }
      }
    }
  }

  if (ts.size() == 1) { // Single point (due to limited precision) => edge intersection
    t0 = ts[0];
    t1 = ts[0];
    return true;
  }
  else { // No point found
    t0 = 0;
    t1 = 0;
    return false;
  }
}

}
