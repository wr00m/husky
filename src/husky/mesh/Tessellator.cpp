#include <husky/mesh/Tessellator.hpp>
#include <husky/Log.hpp>
#include <husky/geo/Shapefile.hpp> // TODO: Include Feature, not Shapefile
#include <glad/glad.h>
#include <gl/GLU.h>

namespace husky {

void Tessellator::tessellate(const Feature &feature, std::vector<Vector3d> &outPts, std::vector<Vector3i> &outTris)
{
  GLUtesselator *tess = gluNewTess();
  if (tess == nullptr) {
    return;
  }

  // TODO: gluTessNormal?

  GLenum type = 0;
  static auto __tessBegin = [&type](GLenum type2) {
    type = type2;
    // TODO
    husky::Log::debug("Begin: %d", type2);
  };
  auto _tessBegin = [](GLenum type) { __tessBegin(type); };
  auto tessBegin = (GLvoid(CALLBACK*)(void))(GLvoid(CALLBACK*)(GLenum))_tessBegin;

  auto _tessVert = [](void *vertData) {
    // TODO
  };
  auto tessVert = (GLvoid(CALLBACK*)())(GLvoid(CALLBACK*)(void *vertData))_tessVert;

  auto _tessCombine = [](void *polyData) {
    // TODO
  };
  auto tessCombine = (GLvoid(CALLBACK*)())(GLvoid(CALLBACK*)(void *polyData))_tessCombine;

  auto _tessEnd = [](void) {
    // TODO
    husky::Log::debug("End");
  };
  auto tessEnd = (GLvoid(CALLBACK*)())_tessEnd;

  gluTessCallback(tess, GLU_TESS_BEGIN, tessBegin);
  gluTessCallback(tess, GLU_TESS_VERTEX, tessVert);
  gluTessCallback(tess, GLU_TESS_COMBINE, tessCombine);
  gluTessCallback(tess, GLU_TESS_END, tessEnd);

  outPts.clear();
  outPts.reserve(feature._points.size());
  for (const auto &pt : feature._points) {
    outPts.emplace_back(pt.xyz);
  }

  //gluTessBeginPolygon(tess, const_cast<void*>(reinterpret_cast<const void*>(&feature)));
  gluTessBeginPolygon(tess, nullptr);

  for (int iPart = 0; iPart < (int)feature._parts.size(); iPart++) {
    gluTessBeginContour(tess);

    int iVertBeginIncl = feature._parts[iPart];
    int iVertEndExcl = (iPart == feature._parts.size() - 1 ? (int)feature._points.size() : feature._parts[iPart + 1]);
    for (int iVert = iVertBeginIncl; iVert < iVertEndExcl; iVert++) {
      gluTessVertex(tess, outPts[iVert].val, outPts[iVert].val);
    }

    gluTessEndContour(tess);
  }

  gluTessEndPolygon(tess);

  gluDeleteTess(tess);
}

}
