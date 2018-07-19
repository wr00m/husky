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

Frustum::IntersectionResult Frustum::touches(const Vector3d &boxMin, const Vector3d &boxMax) const
{
  constexpr unsigned int NUM_CORNERS = 8;

  std::vector<Vector3d> corners;
  corners.reserve(NUM_CORNERS);

  for (unsigned int i = 0; i < NUM_CORNERS; i++) {
    Vector3d corner;
    corner.x = (i < 4     ? boxMax.x : boxMin.x);
    corner.y = (i % 4 < 2 ? boxMax.y : boxMin.y);
    corner.z = (i % 2     ? boxMax.z : boxMin.z);
    corners.emplace_back(corner);
  }

  return touches(corners);
}

Frustum::IntersectionResult Frustum::touches(const Vector3d &sphereCenter, double sphereRadius) const
{
  int planeCount = 0;

  for (unsigned int iPlane = 0; iPlane < NUM_CLIPPING_PLANES; iPlane++) {
    const double dist = getPointDistToPlane(sphereCenter, clippingPlanes[iPlane]);
    if (dist <= -sphereRadius) {
      return IntersectionResult::OUTSIDE;
    }
    else if (dist > sphereRadius) {
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
