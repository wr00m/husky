#pragma once

#include <husky/math/Matrix44.hpp>
#include <husky/mesh/BoundingBox.hpp>
#include <husky/render/RenderData.hpp>
#include <vector>
#include <cassert>

namespace husky {

class SimpleMesh
{
public:
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
  typedef std::vector<Vertex> Vertices;
  typedef std::vector<Triangle> Triangles;
  typedef std::vector<Quad> Quads;

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

  const Vertices& getVertices() const { return verts; }
  const Triangles& getTriangles() const { return tris; }
  const Quads& getQuads() const { return quads; }

  void clear() { verts.clear(); tris.clear(); quads.clear(); }
  void reserve(int vertexCount, int triangleCount, int quadCount) { verts.reserve(vertexCount); tris.reserve(triangleCount); quads.reserve(quadCount); }

  void setAllVertexColors(const Color &color) { for (Vertex &vert : verts) { vert.color = color; } }

  void addMesh(const SimpleMesh &otherMesh)
  {
    reserve(getVertexCount() + otherMesh.getVertexCount(), getTriangleCount() + otherMesh.getTriangleCount(), getQuadCount() + otherMesh.getQuadCount());

    const int indexOffset = getVertexCount();

    for (const Vertex &vert : otherMesh.verts) {
      verts.emplace_back(vert);
    }

    for (const Triangle &tri : otherMesh.tris) {
      tris.emplace_back(tri + indexOffset);
    }

    for (const Quad &quad : otherMesh.quads) {
      quads.emplace_back(quad + indexOffset);
    }
  }

  void triangulateQuads()
  {
    for (const Quad &q : quads) {
      addTriangle(q[0], q[1], q[2]);
      addTriangle(q[0], q[2], q[3]);
    }

    quads.clear();
  }

  void recalculateVertexNormals()
  {
    // Initialize vertex normals to zero
    for (Vertex &vert : verts) {
      vert.normal.set(0.0, 0.0, 0.0);
    }

    // Get vertex normal contributions from triangles
    for (const Triangle &tri : tris) {
      Vertex &v0 = verts[tri[0]];
      Vertex &v1 = verts[tri[1]];
      Vertex &v2 = verts[tri[2]];

      const Normal triNormal = (v1.pos - v0.pos).cross(v2.pos - v0.pos).normalized();

      v0.normal += triNormal;
      v1.normal += triNormal;
      v2.normal += triNormal;
    }

    // Get vertex normal contributions from quads
    for (const Quad &quad : quads) {
      Vertex &v0 = verts[quad[0]];
      Vertex &v1 = verts[quad[1]];
      Vertex &v2 = verts[quad[2]];
      Vertex &v3 = verts[quad[3]];

      const Normal quadNormal
        = ((v1.pos - v0.pos).cross(v3.pos - v0.pos)
        +  (v2.pos - v1.pos).cross(v0.pos - v1.pos)
        +  (v3.pos - v2.pos).cross(v1.pos - v2.pos)
        +  (v0.pos - v3.pos).cross(v2.pos - v3.pos)).normalized();

      v0.normal += quadNormal;
      v1.normal += quadNormal;
      v2.normal += quadNormal;
      v3.normal += quadNormal;
    }

    // Normalize vertex normals
    for (Vertex &vert : verts) {
      vert.normal.normalize();
    }
  }

  void transform(const Matrix44d &m)
  {
    const Matrix33d nm = m.get3x3(); // Normal matrix

    for (Vertex &vert : verts) {
      vert.pos = (m * Vector4d(vert.pos, 1.0)).xyz;
      vert.normal = nm * vert.normal;
    }
  }

  RenderData getRenderData() const
  {
    RenderData r(RenderData::Mode::TRIANGLES);
    r.addAttr(RenderData::Attribute::POSITION, sizeof(Vector3f));
    r.addAttr(RenderData::Attribute::NORMAL,   sizeof(Vector3f));
    r.addAttr(RenderData::Attribute::TEXCOORD, sizeof(Vector2f));
    r.addAttr(RenderData::Attribute::COLOR,    sizeof(Vector4b));
    r.init((int)verts.size());

    if (!verts.empty()) {
      r.anchor = Vector3f(0, 0, 0); //(Vector3f)verts.front().pos;

      for (int i = 0; i < verts.size(); i++) {
        const Vertex &vert = verts[i];
        r.setValue(i, RenderData::Attribute::POSITION, (Vector3f)vert.pos);
        r.setValue(i, RenderData::Attribute::NORMAL, (Vector3f)vert.normal);
        r.setValue(i, RenderData::Attribute::TEXCOORD, (Vector2f)vert.texCoord);
        r.setValue(i, RenderData::Attribute::COLOR, vert.color);
      }

      for (const Triangle &t : tris) {
        r.addTriangle(t[0], t[1], t[2]);
      }

      for (const Quad &q : quads) {
        r.addTriangle(q[0], q[1], q[2]);
        r.addTriangle(q[0], q[2], q[3]);
      }
    }

    return r;
  }

  RenderData getRenderDataWireframe() const
  {
    RenderData r(RenderData::Mode::LINES);
    r.addAttr(RenderData::Attribute::POSITION, sizeof(Vector3f));
    r.addAttr(RenderData::Attribute::COLOR,    sizeof(Vector4b));
    r.init((int)verts.size());

    if (!verts.empty()) {
      r.anchor = Vector3f(0, 0, 0); //(Vector3f)verts.front().pos;

      for (int i = 0; i < verts.size(); i++) {
        const Vertex &vert = verts[i];
        r.setValue(i, RenderData::Attribute::POSITION, (Vector3f)vert.pos);
        r.setValue(i, RenderData::Attribute::COLOR, vert.color);
      }

      for (const Triangle &t : tris) {
        r.addLine(t[0], t[1]);
        r.addLine(t[1], t[2]);
        r.addLine(t[2], t[0]);
      }

      for (const Quad &q : quads) {
        r.addLine(q[0], q[1]);
        r.addLine(q[1], q[2]);
        r.addLine(q[2], q[3]);
        r.addLine(q[3], q[0]);
      }
    }

    return r;
  }

  BoundingBox getBoundingBox() const
  {
    BoundingBox box;
    if (verts.empty()) {
      return box;
    }

    box.init(verts.front().pos);
    for (const Vertex &vert : verts) {
      box.expand(vert.pos);
    }

    return box;
  }

private:
  Vertices verts;
  Triangles tris;
  Quads quads;
};

}
