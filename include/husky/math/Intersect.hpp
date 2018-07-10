#pragma once

#include <husky/math/Vector3.hpp>

namespace husky {

class HUSKY_DLL Intersect
{
public:
  static int  lineIntersectsPlane(const Vector3d &linePt, const Vector3d &lineDir, const Vector3d &planePt, const Vector3d &planeNormal, double &t, double tolerance = 1e-4);
  static int  lineIntersectsTriangle(const Vector3d &linePt, const Vector3d &lineDir, const Vector3d &v0, const Vector3d &v1, const Vector3d &v2, double &t);
  static int  lineIntersectsQuad(const Vector3d &linePt, const Vector3d &lineDir, const Vector3d &v0, const Vector3d &v1, const Vector3d &v2, const Vector3d &v3, double &t);
  static bool lineIntersectsSphere(const Vector3d &linePt, const Vector3d &lineDir, const Vector3d &sphereCenter, double sphereRadius, double &t0, double &t1);
  static bool lineIntersectsBox(const Vector3d &linePt, const Vector3d &lineDir, const Vector3d &boxMin, const Vector3d &boxMax, double &t0, double &t1);
};

}
