#include <husky/mesh/Tessellator.hpp>
#include <husky/geo/Shapefile.hpp>
#include <glad/glad.h>
#include <gl/GLU.h>

namespace husky {

void Tessellator::tessellate(const Feature &feature, std::vector<Vector3d> &outPts, std::vector<Vector3i> &outTris)
{
  GLUtesselator *tess = gluNewTess();
  if (tess == nullptr) {
    return;
  }

  gluTessBeginPolygon(tess, nullptr);

  // TODO

  //for (const )
  //gluTessBeginContour(tess);
  //gluTessVertex(tess, , );
  //gluTessEndContour(tess);

  gluTessEndPolygon(tess);

  gluDeleteTess(tess);
}

}
