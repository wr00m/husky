#pragma once

#include <husky/math/Vector4.hpp>
#include <vector>

namespace husky {

class Feature;

class HUSKY_DLL Triangulator
{
public:
  static void tessellateGlu(const Feature &feature, std::vector<Vector3d> &outPts, std::vector<Vector3i> &outTris);
  static void tessellate(const Feature &feature, std::vector<Vector3d> &outPts, std::vector<Vector3i> &outTris);

  Triangulator(const Vector2d &boundsMin, const Vector2d &boundsMax);

  void init(const Vector2d &boundsMin, const Vector2d &boundsMax);
  void addPoint(const Vector2d &pt);
  int triFind(const Vector2d &pt, int t) const;
  void splitTriangle(int iTri, int iVert);

//private:
  //struct Vertex
  //{
  //  double x;
  //  double y;
  //  union {
  //    int _neighborTris[]
  //  };
  //};

  struct Triangle
  {
    Triangle(int v0, int v1, int v2);
    Triangle(int v0, int v1, int v2, int n0, int n1, int n2);

    union {
      int _verts[3] = { -1, -1, -1 }; // Vertices ordered CCW
      struct { int v0, v1, v2; };
    };
    union {
      int _neighbors[3] = { -1, -1, -1 }; // Neighbor triangles ordered CCW
      struct { int n0, n1, n2; };
    };
  };

  std::vector<Vector2d> _pts;
  std::vector<Triangle> _tris;
};

}
