#pragma once

#include <husky/math/Matrix44.hpp>
#include <husky/math/Box.hpp>
#include <husky/math/Sphere.hpp>

namespace husky {

class HUSKY_DLL Frustum
{
public:
  enum class IntersectionResult { OUTSIDE, INSIDE, INTERSECTING, };

  Frustum();
  Frustum(const Matrix44d &mtxMvp);
  Frustum(const Matrix44d &mtxProjection, const Matrix44d &mtxModelView);

  IntersectionResult touches(const Vector3d &pt) const;
  IntersectionResult touches(const Box &box, const Matrix44d *boxTransform = nullptr) const;
  IntersectionResult touches(const Sphere &sphere) const;
  IntersectionResult touches(const std::vector<Vector3d> &polyPts) const;

private:
  static double getPointDistToPlane(const Vector3d &pt, const Vector4d &plane);

  static constexpr unsigned int NUM_CLIPPING_PLANES = 6;
  Vector4d clippingPlanes[NUM_CLIPPING_PLANES];
};

}
