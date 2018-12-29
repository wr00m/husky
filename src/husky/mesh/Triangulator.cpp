#include <husky/mesh/Triangulator.hpp>
#include <husky/math/Box.hpp>
#include <husky/Log.hpp>
#include <husky/geo/Shapefile.hpp> // TODO: Include Feature, not Shapefile
#include <glad/glad.h>
#include <gl/GLU.h>
//#include <memory>
#include <algorithm>
#include <set>

namespace husky {

void Triangulator::tessellateGlu(const Feature &feature, std::vector<Vector3d> &outPts, std::vector<Vector3i> &outTris)
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
        int i = (int)outPts.size();
        outTris.emplace_back(i - 2, i - 3, i - 1);
      }
    }
    else if (type == GL_TRIANGLE_FAN) {
      if (vertCount >= 3) {
        int i = (int)outPts.size();
        outTris.emplace_back(i - 2, i - vertCount, i - 1);
      }
    }
    else if (type == GL_TRIANGLE_STRIP) {
      if (vertCount >= 3) {
        int i = (int)outPts.size();
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

void Triangulator::tessellate(const Feature &feature, std::vector<Vector3d> &outPts, std::vector<Vector3i> &outTris)
{
  outPts.clear();
  outTris.clear();

  Triangulator triangulator(feature._bboxMin.xy, feature._bboxMax.xy);

  for (int iPart = 0; iPart < (int)feature._parts.size(); iPart++) {
    int iVertBeginIncl = feature._parts[iPart];
    int iVertEndExcl = (iPart == feature._parts.size() - 1 ? (int)feature._points.size() : feature._parts[iPart + 1]);
    for (int iVert = iVertBeginIncl; iVert < iVertEndExcl; iVert++) {
      triangulator.addPoint(feature._points[iVert].xy);
    }
    for (const auto &pt : triangulator._pts) {
      outPts.emplace_back(pt, 0.0);
    }
    for (const auto &tri : triangulator._tris) {
      outTris.emplace_back(tri.v0, tri.v1, tri.v2);
    }
    break; // TODO: Handle interior rings and multiple exterior rings
  }
}

Triangulator::Triangle::Triangle(int v0, int v1, int v2)
  : _verts{ v0, v1, v2 }
  , _neighbors{ -1, -1, -1 }
{
}

Triangulator::Triangle::Triangle(int v0, int v1, int v2, int n0, int n1, int n2)
  : _verts{ v0, v1, v2 }
  , _neighbors{ n0, n1, n2 }
{
}

Triangulator::Triangulator(const Vector2d &boundsMin, const Vector2d &boundsMax)
{
  init(boundsMin, boundsMax);
}

void Triangulator::init(const Vector2d &boundsMin, const Vector2d &boundsMax)
{
  auto s = (boundsMax - boundsMin) * 2.0;
  
  // Add supertriangle points
  double eps = (s.x + s.y) * 0.5 * 1e-9;
  _pts.clear();
  _pts.emplace_back(Vector2d( -eps,  -eps) * s + boundsMin);
  _pts.emplace_back(Vector2d(1+eps,  -eps) * s + boundsMin);
  _pts.emplace_back(Vector2d( -eps, 1+eps) * s + boundsMin);
  
  // Add supertriangle
  _tris.clear();
  _tris.emplace_back(0, 1, 2);
}

void Triangulator::addPoint(const Vector2d &pt)
{
  _pts.emplace_back(pt);
  int iPt = (int)_pts.size() - 1;

  int t = (int)_tris.size() - 1; // Start search at last triangle created
  t = triFind(pt, t);

  // Split triangle into three new ones
  if (t == -1) { // TODO: Handle point located on an existing edge/point?
    Log::warning("No triangle found! (Did you add a point outside the specified bounds?)");
  }
  else {
    splitTriangle(t, iPt);
  }
}

int Triangulator::triFind(const Vector2d &pt, int t) const
{
  // http://www.personal.psu.edu/cxc11/AERSP560/DELAUNEY/13_Two_algorithms_Delauney.pdf
  while (true) {
    for (int i = 0; i < 3; i++) {
      int j = (i == 2 ? 0 : (i + 1));
      const auto &tri = _tris[t];
      const auto &a = _pts[tri._verts[i]];
      const auto &b = _pts[tri._verts[j]];

      // Point-to-edge distance
      // https://math.stackexchange.com/a/274728
      double d = (pt.x - a.x) * (b.y - a.y) - (pt.y - a.y) * (b.x - a.x);

      if (std::abs(d) < 1e-9) {
        Log::warning("Point on edge"); // TODO: Handle
      }

      if (d > 0) { // Point on far side of edge
        t = tri._neighbors[i]; // Continue search from neighbor triangle
        break;
      }

      if (i == 2) { // Did not continue search
        return t;
      }
    }
  }
}

void Triangulator::splitTriangle(int iTri, int iVert)
{
  const Triangle o = _tris[iTri]; // Original triangle

  int t0 = iTri; // Overwrite original triangle
  int t1 = (int)_tris.size();
  int t2 = t1 + 1;

  _tris[iTri] = Triangle(o.v0, o.v1, iVert, o.n0, t1, t2);
  _tris.emplace_back(    o.v1, o.v2, iVert, o.n1, t2, t0);
  _tris.emplace_back(    o.v2, o.v0, iVert, o.n2, t0, t1);
}

}
