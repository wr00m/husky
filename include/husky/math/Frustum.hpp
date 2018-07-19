#pragma once

#include <husky/math/Matrix44.hpp>
#include <vector>

namespace husky
{

class HUSKY_DLL Frustum
{
public:
  enum class IntersectionResult { OUTSIDE, INSIDE, INTERSECTING, };

  Frustum();
  Frustum(const Matrix44d &mtxMvp);
  Frustum(const Matrix44d &mtxProjection, const Matrix44d &mtxModelView);

  IntersectionResult touches(const Vector3d &pt) const;
  IntersectionResult touches(const Vector3d &boxMin, const Vector3d &boxMax) const;
  IntersectionResult touches(const Vector3d &sphereCenter, double sphereRadius) const;
  IntersectionResult touches(const std::vector<Vector3d> &polyPts) const;

private:
  static double getPointDistToPlane(const Vector3d &pt, const Vector4d &plane);

  static constexpr unsigned int NUM_CLIPPING_PLANES = 6;
  Vector4d clippingPlanes[NUM_CLIPPING_PLANES];
};

}
