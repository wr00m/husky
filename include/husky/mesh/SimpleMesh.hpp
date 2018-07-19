#pragma once

#include <husky/math/Matrix44.hpp>
#include <husky/render/RenderData.hpp>
#include <vector>

namespace husky {

class HUSKY_DLL SimpleMesh
{
public:
  static SimpleMesh box(double sizeX, double sizeY, double sizeZ);
  static SimpleMesh cylinder(double radius, double height, bool cap = false, int segmentCount = 16);
  static SimpleMesh sphere(double radius, int uSegmentCount = 32, int vSegmentCount = 16);
  static SimpleMesh torus(double innerRadius, double outerRadius, int uSegmentCount = 32, int vSegmentCount = 16);
  //static SimpleMesh capsule();

  typedef Vector3d Position;
  typedef Vector3d Normal;
  typedef Vector2d TexCoord;
  typedef Vector4b Color;

  struct Vertex
  {
    Vertex(const Position &pos)
      : pos(pos)
      , normal(0.0, 0.0, 0.0)
      , texCoord(0.0, 0.0)
      , color(255, 255, 255, 255)
    {
    }

    Position pos;
    Normal normal;
    TexCoord texCoord;
    Color color;
  };

  typedef Vector3i Triangle;
  typedef Vector4i Quad;

  int getVertexCount() const { return int(verts.size()); }
  int getTriangleCount() const { return int(tris.size()); }
  int getQuadCount() const { return int(quads.size()); }
  int addVertex(const Vertex &v) { verts.emplace_back(v); return int(verts.size() - 1); }
  int addVertex(const Position &v) { verts.emplace_back(v); return int(verts.size() - 1); }
  int addVertex(const Position &v, const Normal &n, const TexCoord &texCoord) { verts.emplace_back(v); verts.back().normal = n; verts.back().texCoord = texCoord; return int(verts.size() - 1); }
  void addTriangle(const Triangle &t) { tris.emplace_back(t); }
  void addTriangle(int v0, int v1, int v2) { tris.emplace_back(v0, v1, v2); }
  Triangle addTriangle(const Position &v0, const Position &v1, const Position &v2) { tris.emplace_back(addVertex(v0), addVertex(v1), addVertex(v2)); return tris.back(); }
  void addQuad(const Quad &q) { quads.emplace_back(q); }
  void addQuad(int v0, int v1, int v2, int v3) { quads.emplace_back(v0, v1, v2, v3); }
  Quad addQuad(const Position &v0, const Position &v1, const Position &v2, const Position &v3) { quads.emplace_back(addVertex(v0), addVertex(v1), addVertex(v2), addVertex(v3)); return quads.back(); }
  void setNormal(int i, const Normal &n) { verts[i].normal = n; }
  void setTexCoord(int i, double u, double v) { verts[i].texCoord.set(u, v); }
  void setTexCoord(int i, const TexCoord &uv) { verts[i].texCoord = uv; }
  const Vertex& getVertex(int i) const { return verts[i]; }
  const Triangle& getTriangle(int i) const { return tris[i]; }
  const Quad& getQuad(int i) const { return quads[i]; }
  const std::vector<Vertex>& getVertices() const { return verts; }
  const std::vector<Triangle>& getTriangles() const { return tris; }
  const std::vector<Quad>& getQuads() const { return quads; }
  void clear() { verts.clear(); tris.clear(); quads.clear(); }
  void reserve(int vertexCount, int triangleCount, int quadCount) { verts.reserve(vertexCount); tris.reserve(triangleCount); quads.reserve(quadCount); }
  void setAllVertexColors(const Color &color) { for (Vertex &vert : verts) { vert.color = color; } }
  void addMesh(const SimpleMesh &otherMesh);
  void triangulateQuads();
  void recalculateVertexNormals();
  void transform(const Matrix44d &m);
  RenderData getRenderData() const;
  RenderData getRenderDataWireframe() const;

private:
#pragma warning(suppress: 4251)
  std::vector<Vertex> verts;
#pragma warning(suppress: 4251)
  std::vector<Triangle> tris;
#pragma warning(suppress: 4251)
  std::vector<Quad> quads;
};

}
