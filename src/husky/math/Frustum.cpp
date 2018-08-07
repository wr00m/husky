#include <husky/math/Frustum.hpp>

namespace husky {

Frustum::Frustum()
  : Frustum(Matrix44d::identity())
{
}

Frustum::Frustum(const Matrix44d &mtxMvp)
{
  // Calculate clipping planes
  clippingPlanes[0] = mtxMvp.row(3) - mtxMvp.row(0); // Right
  clippingPlanes[1] = mtxMvp.row(3) + mtxMvp.row(0); // Left
  clippingPlanes[2] = mtxMvp.row(3) - mtxMvp.row(2); // Far
  clippingPlanes[3] = mtxMvp.row(3) + mtxMvp.row(2); // Near
  clippingPlanes[4] = mtxMvp.row(3) - mtxMvp.row(1); // Top
  clippingPlanes[5] = mtxMvp.row(3) + mtxMvp.row(1); // Bottom

  // Normalize clipping planes
  for (unsigned int i = 0; i < NUM_CLIPPING_PLANES; i++) {
    const double normalLength = clippingPlanes[i].xyz.length();
    if (normalLength != 0) {
      clippingPlanes[i] /= normalLength;
    }
  }
}

Frustum::Frustum(const Matrix44d &mtxProjection, const Matrix44d &mtxModelView)
  : Frustum(mtxProjection * mtxModelView)
{
}

double Frustum::getPointDistToPlane(const Vector3d &pt, const Vector4d &plane)
{
  return (plane.xyz.dot(pt) + plane.w);
}

Frustum::IntersectionResult Frustum::touches(const Vector3d &pt) const
{
  for (unsigned int iPlane = 0; iPlane < NUM_CLIPPING_PLANES; iPlane++) {
    if (getPointDistToPlane(pt, clippingPlanes[iPlane]) <= 0.0) {
      return IntersectionResult::OUTSIDE;
    }
  }
  return IntersectionResult::INSIDE;
}

Frustum::IntersectionResult Frustum::touches(const Box &box, const Matrix44d *boxTransform) const
{
  std::vector<Vector3d> corners = box.corners();
  if (boxTransform != nullptr) {
    for (Vector3d &corner : corners) {
      corner = (*boxTransform * Vector4d(corner, 1)).xyz;
    }
  }
  return touches(corners);
}

Frustum::IntersectionResult Frustum::touches(const Sphere &sphere) const
{
  int planeCount = 0;

  for (unsigned int iPlane = 0; iPlane < NUM_CLIPPING_PLANES; iPlane++) {
    const double dist = getPointDistToPlane(sphere.center, clippingPlanes[iPlane]);
    if (dist <= -sphere.radius) {
      return IntersectionResult::OUTSIDE;
    }
    else if (dist > sphere.radius) {
      planeCount++;
    }
  }

  return (planeCount == NUM_CLIPPING_PLANES ? IntersectionResult::INSIDE : IntersectionResult::INTERSECTING);
}

Frustum::IntersectionResult Frustum::touches(const std::vector<Vector3d> &polyPts) const
{
  int planeCount = 0;

  for (unsigned int iPlane = 0; iPlane < NUM_CLIPPING_PLANES; iPlane++) {
    unsigned int pointCount = 0;
    for (unsigned int iPt = 0; iPt < polyPts.size(); iPt++) {
      if (getPointDistToPlane(polyPts[iPt], clippingPlanes[iPlane]) > 0.0) {
        pointCount++;
      }
    }

    if (pointCount == 0) {
      return IntersectionResult::OUTSIDE;
    }
    else if (pointCount == polyPts.size()) {
      planeCount++;
    }
  }

  return (planeCount == NUM_CLIPPING_PLANES ? IntersectionResult::INSIDE : IntersectionResult::INTERSECTING);
}

}
