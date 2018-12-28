#include <husky/mesh/Tessellator.hpp>
#include <husky/math/Box.hpp>
#include <husky/Log.hpp>
#include <husky/geo/Shapefile.hpp> // TODO: Include Feature, not Shapefile
#include <glad/glad.h>
#include <gl/GLU.h>
//#include <memory>
#include <algorithm>
#include <set>

namespace husky {

void Tessellator::tessellate(const Feature &feature, std::vector<Vector3d> &outPts, std::vector<Vector3i> &outTris)
{
#if 1
  outPts.clear();
  outTris.clear();

  for (int iPart = 0; iPart < (int)feature._parts.size(); iPart++) {
    int iVertBeginIncl = feature._parts[iPart];
    int iVertEndExcl = (iPart == feature._parts.size() - 1 ? (int)feature._points.size() : feature._parts[iPart + 1]);
    std::vector<Vector4d> partPts;
    for (int iVert = iVertBeginIncl; iVert < iVertEndExcl; iVert++) {
      partPts.emplace_back(feature._points[iVert]);
    }
    std::vector<Vector4d> outPts4;
    husky::Tessellator::constrainedDelaunayTriangulation(partPts, outPts4, outTris);
    for (const auto &pt : outPts4) {
      outPts.emplace_back(pt.xyz);
    }
    break; // TODO: Handle interior rings and multiple exterior rings
  }
#else
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
  #endif
}

#if 1
static double pointInTriangleSign(const Vector2d &p1, const Vector2d &p2, const Vector2d &p3)
{
  return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

static bool pointInTriangle(const Vector2d &pt, const Vector2d &v1, const Vector2d &v2, const Vector2d &v3)
{
  // https://stackoverflow.com/a/2049593/4175119

  double d1 = pointInTriangleSign(pt, v1, v2);
  double d2 = pointInTriangleSign(pt, v2, v3);
  double d3 = pointInTriangleSign(pt, v3, v1);

  bool hasNegative = (d1 < 0) || (d2 < 0) || (d3 < 0);
  bool hasPositive = (d1 > 0) || (d2 > 0) || (d3 > 0);

  return !(hasNegative && hasPositive);
}
#else
static bool pointInTriangle(const Vector2d &P, const Vector2d &A, const Vector2d &B, const Vector2d &C)
{
  // Compute vectors        
  auto v0 = C - A;
  auto v1 = B - A;
  auto v2 = P - A;

  // Compute dot products
  auto dot00 = v0.dot(v0);
  auto dot01 = v0.dot(v1);
  auto dot02 = v0.dot(v2);
  auto dot11 = v1.dot(v1);
  auto dot12 = v1.dot(v2);

  // Compute barycentric coordinates
  auto invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
  auto u = (dot11 * dot02 - dot01 * dot12) * invDenom;
  auto v = (dot00 * dot12 - dot01 * dot02) * invDenom;

  // Check if point is in triangle
  return (u >= 0) && (v >= 0) && (u + v < 1);
}
#endif

struct HalfEdge
{
  HalfEdge(int vert, int nextEdge, int twinEdge, int tri)
    : _vert(vert)
    , _nextEdge(nextEdge)
    , _twinEdge(twinEdge)
    , _tri(tri)
  {
  }

  int _vert;
  int _nextEdge;
  int _twinEdge;
  int _tri;
};

struct HalfEdgeTriangleMesh
{
  void splitTriangle(int iTri, int iVert) {
    int e0 = _tris[iTri][0];
    int e1 = _tris[iTri][1];
    int e2 = _tris[iTri][2];
    int e3 = (int)_edges.size();
    int e4 = e3 + 1;
    int e5 = e3 + 2;
    int e6 = e3 + 3;
    int e7 = e3 + 4;
    int e8 = e3 + 5;

    int v0 = _edges[e0]._vert;
    int v1 = _edges[e1]._vert;
    int v2 = _edges[e2]._vert;
    int v3 = iVert;

    int t0 = iTri;
    int t1 = (int)_tris.size();
    int t2 = t1 + 1;

    _edges[e0] = { v0, e3, _edges[e0]._twinEdge, t0 }; // e0
    _edges[e1] = { v1, e5, _edges[e1]._twinEdge, t1 }; // e1
    _edges[e2] = { v2, e7, _edges[e2]._twinEdge, t2 }; // e2
    _edges.emplace_back(v1, e4, e6, t0); // e3
    _edges.emplace_back(v3, e0, e7, t0); // e4
    _edges.emplace_back(v2, e6, e8, t1); // e5
    _edges.emplace_back(v3, e1, e3, t1); // e6
    _edges.emplace_back(v0, e8, e4, t2); // e7
    _edges.emplace_back(v3, e2, e5, t2); // e8

    _tris[iTri].set(e0, e3, e4); // Re-use original triangle (to be removed)
    _tris.emplace_back(e1, e5, e6);
    _tris.emplace_back(e2, e7, e8);
  }

  int findTriangleContainingPoint(const Vector2d &pt, const std::vector<Vector3d> &pts, std::vector<int> &outerTris, std::set<int> &visitedTris) const {
    while (!outerTris.empty()) {
      std::vector<int> outerTrisCopy = outerTris;
      outerTris.clear();
      for (int iTri : outerTrisCopy) {
        if (iTri == -1) {
          continue; // Triangle does not exist
        }
        if (!visitedTris.insert(iTri).second) { // Mark triangle as visited
          continue; // Triangle already visited
        }
        const auto &tri = _tris[iTri];
        if (pointInTriangle(pt, pts[_edges[tri[0]]._vert].xy, pts[_edges[tri[1]]._vert].xy, pts[_edges[tri[2]]._vert].xy)) {
          return iTri;
        }
        for (int iNeighborEdge = 0; iNeighborEdge < 3; iNeighborEdge++) {
          //if (tri[iNeighborEdge] == -1) {
          //  continue;
          //}
          const auto &edge = _edges[tri[iNeighborEdge]];
          if (edge._twinEdge == -1) {
            continue;
          }
          int iNeighborTri = _edges[edge._twinEdge]._tri;
          if (iNeighborTri != -1) {
            outerTris.emplace_back(iNeighborTri);
          }
        }
      }
    }
    return -1;
  }

  std::vector<HalfEdge> _edges;
  std::vector<Vector3i> _tris; // Half-edge indices
};

void Tessellator::constrainedDelaunayTriangulation(const std::vector<Vector4d> &inPts, std::vector<Vector4d> &outPts, std::vector<Vector3i> &outTris)
{
  // https://www.newcastle.edu.au/__data/assets/pdf_file/0019/22519/23_A-fast-algortithm-for-generating-constrained-Delaunay-triangulations.pdf

  Box bbox;
  for (const auto &pt : inPts) {
    bbox.expand(pt.xyz);
  }

  Vector2d scale = Vector2d(1.0) / bbox.size().xy; // TODO: Prevent divide by zero
  Vector2d bias = -bbox.min.xy;

  // Sort points by bin
  double binCountTotal = std::sqrt((double)inPts.size());
  int binCountXY = (int)std::sqrt(binCountTotal);
  auto sortedPts = inPts;
  for (auto &pt : sortedPts) {
    pt.xy = (pt.xy + bias) * scale; // Normalize point
  }
  std::sort(outPts.begin(), outPts.end(), [binCountXY](const Vector4d &lhs, const Vector4d &rhs) {
    Vector2i lhsBinIndex((int)(lhs.x * binCountXY), (int)(lhs.y * binCountXY));
    Vector2i rhsBinIndex((int)(rhs.x * binCountXY), (int)(rhs.y * binCountXY));
    if (lhsBinIndex.x < rhsBinIndex.x) {
      return true;
    }
    else if (lhsBinIndex.x > rhsBinIndex.x) {
      return false;
    }
    else if (lhsBinIndex.y < rhsBinIndex.y) {
      return true;
    }
    //else if (lhsBinIndex.y > rhsBinIndex.y) {
    //  return false;
    //}
    else {
      return false;
    }
  });

  std::vector<Vector3d> triPts;
  triPts.emplace_back(-0.0001, -0.0001, 0); // Supertriangle vertices
  triPts.emplace_back( 2.0001, -0.0001, 0);
  triPts.emplace_back(-0.0001,  2.0001, 0);

  HalfEdgeTriangleMesh m;
  m._edges.emplace_back(0, 1, -1, 0);
  m._edges.emplace_back(1, 2, -1, 0);
  m._edges.emplace_back(2, 0, -1, 0);

  m._tris.emplace_back(0, 1, 2); // Supertriangle

  for (const auto &pt : sortedPts) {
    // Find triangle containing new point
    std::vector<int> outerTris = { (int)m._tris.size() - 1 };
    std::set<int> visitedTris;
    int iTri = m.findTriangleContainingPoint(pt.xy, triPts, outerTris, visitedTris);

    husky::Log::debug("Visited: %d", visitedTris.size());

    // Add new point
    int iPt = (int)triPts.size();
    triPts.emplace_back(pt.xyz);

    // Split triangle into three new ones
    if (iTri == -1) { // TODO: Handle point located on an existing edge/point?
      Log::warning("No triangle found!");
    }
    else {
      m.splitTriangle(iTri, iPt);
    }

    // Initialize stack
    // TODO
  }

  outTris.clear();
  for (const auto &tri : m._tris) {
    if (m._edges[tri[0]]._vert < 3 || m._edges[tri[1]]._vert < 3 || m._edges[tri[2]]._vert < 3) {
      continue; // Skip remains of supertriangle
    }

    outTris.emplace_back(m._edges[tri[0]]._vert, m._edges[tri[1]]._vert, m._edges[tri[2]]._vert); // -3);
  }

  outPts.clear();
  outPts.reserve(triPts.size());
  for (auto &pt : triPts) {
    outPts.emplace_back((pt.xy / scale) - bias, 0.0, 0.0); // De-normalize point
  }
}

}
