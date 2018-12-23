#include <husky/mesh/Tessellator.hpp>
#include <husky/Log.hpp>
#include <husky/geo/Shapefile.hpp> // TODO: Include Feature, not Shapefile
#include <glad/glad.h>
#include <gl/GLU.h>

namespace husky {

void Tessellator::tessellate(const Feature &feature, std::vector<Vector3d> &outPts, std::vector<Vector3i> &outTris)
{
  outPts.clear();
  outTris.clear();

  GLUtesselator *tess = gluNewTess();
  if (tess == nullptr) {
    return;
  }

  // TODO: gluTessNormal?

  GLenum type = 0;
  int vertCount = 0;
  static auto __tessBegin = [&type, &vertCount](GLenum type2) {
    type = type2;
    vertCount = 0;
    //husky::Log::debug("Begin: %d", type2); // TODO
  };
  auto _tessBegin = [](GLenum type) { __tessBegin(type); };
  auto tessBegin = (GLvoid(CALLBACK*)(void))(GLvoid(CALLBACK*)(GLenum))_tessBegin;

  static auto __tessVert = [&outPts, &outTris, &type, &vertCount](void *vertData) {
    outPts.emplace_back(*reinterpret_cast<Vector3d*>(vertData));
    //husky::Log::debug("  %d => %f %f %f", type, outPts.back().x, outPts.back().y, outPts.back().z); // TODO

    vertCount++;

    if (type == GL_TRIANGLES) {
      if (vertCount % 3 == 0) {
        int i = outPts.size();
        outTris.emplace_back(i - 2, i - 3, i - 1);
      }
    }
    else if (type == GL_TRIANGLE_FAN) {
      if (vertCount >= 3) {
        int i = outPts.size();
        outTris.emplace_back(i - 2, i - vertCount, i - 1);
      }
    }
    else if (type == GL_TRIANGLE_STRIP) {
      if (vertCount >= 3) {
        int i = outPts.size();
        if (vertCount % 2 == 0) {
          outTris.emplace_back(i - 3, i - 2, i - 1);
        }
        else {
          outTris.emplace_back(i - 2, i - 3, i - 1);
        }
      }
    }
    //else ?
  };
  auto _tessVert = [](void *vertData) { __tessVert(vertData); };
  auto tessVert = (GLvoid(CALLBACK*)())(GLvoid(CALLBACK*)(void *vertData))_tessVert;

  auto _tessCombine = [](void *polyData) {
    // TODO
  };
  auto tessCombine = (GLvoid(CALLBACK*)())(GLvoid(CALLBACK*)(void *polyData))_tessCombine;

  auto _tessEnd = [](void) {
    //husky::Log::debug("End"); // TODO
  };
  auto tessEnd = (GLvoid(CALLBACK*)())_tessEnd;

  gluTessCallback(tess, GLU_TESS_BEGIN, tessBegin);
  gluTessCallback(tess, GLU_TESS_VERTEX, tessVert);
  gluTessCallback(tess, GLU_TESS_COMBINE, tessCombine);
  gluTessCallback(tess, GLU_TESS_END, tessEnd);

  std::vector<Vector3d> inPts;
  inPts.reserve(feature._points.size());
  for (const auto &pt : feature._points) {
    inPts.emplace_back(pt.xyz);
  }

  //gluTessBeginPolygon(tess, const_cast<void*>(reinterpret_cast<const void*>(&feature)));
  gluTessBeginPolygon(tess, nullptr);

  for (int iPart = 0; iPart < (int)feature._parts.size(); iPart++) {
    gluTessBeginContour(tess);

    int iVertBeginIncl = feature._parts[iPart];
    int iVertEndExcl = (iPart == feature._parts.size() - 1 ? (int)feature._points.size() : feature._parts[iPart + 1]);
    for (int iVert = iVertBeginIncl; iVert < iVertEndExcl; iVert++) {
      gluTessVertex(tess, inPts[iVert].val, inPts[iVert].val);
    }

    gluTessEndContour(tess);
  }

  gluTessEndPolygon(tess);

  gluDeleteTess(tess);
}

}
