#include <husky/mesh/SimpleMesh.hpp>
#include <husky/math/Math.hpp>

namespace husky {

SimpleMesh SimpleMesh::box(double sizeX, double sizeY, double sizeZ)
{
  const Vector3d h(sizeX * 0.5, sizeY * 0.5, sizeZ * 0.5); // Half size

  SimpleMesh m;

  m.addQuad( // X+
    m.addVertex({  h.x, -h.y, -h.z }, {  1,  0,  0 }, { 0, 0 }),
    m.addVertex({  h.x,  h.y, -h.z }, {  1,  0,  0 }, { 1, 0 }),
    m.addVertex({  h.x,  h.y,  h.z }, {  1,  0,  0 }, { 1, 1 }),
    m.addVertex({  h.x, -h.y,  h.z }, {  1,  0,  0 }, { 0, 1 }));

  m.addQuad( // X-
    m.addVertex({ -h.x,  h.y, -h.z }, { -1,  0,  0 }, { 0, 0 }),
    m.addVertex({ -h.x, -h.y, -h.z }, { -1,  0,  0 }, { 1, 0 }),
    m.addVertex({ -h.x, -h.y,  h.z }, { -1,  0,  0 }, { 1, 1 }),
    m.addVertex({ -h.x,  h.y,  h.z }, { -1,  0,  0 }, { 0, 1 }));

  m.addQuad( // Y+
    m.addVertex({  h.x,  h.y, -h.z }, {  0,  1,  0 }, { 0, 0 }),
    m.addVertex({ -h.x,  h.y, -h.z }, {  0,  1,  0 }, { 1, 0 }),
    m.addVertex({ -h.x,  h.y,  h.z }, {  0,  1,  0 }, { 1, 1 }),
    m.addVertex({  h.x,  h.y,  h.z }, {  0,  1,  0 }, { 0, 1 }));

  m.addQuad( // Y-
    m.addVertex({ -h.x, -h.y, -h.z }, {  0, -1,  0 }, { 0, 0 }),
    m.addVertex({  h.x, -h.y, -h.z }, {  0, -1,  0 }, { 1, 0 }),
    m.addVertex({  h.x, -h.y,  h.z }, {  0, -1,  0 }, { 1, 1 }),
    m.addVertex({ -h.x, -h.y,  h.z }, {  0, -1,  0 }, { 0, 1 }));

  m.addQuad( // Z+
    m.addVertex({ -h.x, -h.y,  h.z }, {  0,  0,  1 }, { 0, 0 }),
    m.addVertex({  h.x, -h.y,  h.z }, {  0,  0,  1 }, { 1, 0 }),
    m.addVertex({  h.x,  h.y,  h.z }, {  0,  0,  1 }, { 1, 1 }),
    m.addVertex({ -h.x,  h.y,  h.z }, {  0,  0,  1 }, { 0, 1 }));

  m.addQuad( // Z-
    m.addVertex({  h.x, -h.y, -h.z }, {  0,  0, -1 }, { 1, 0 }),
    m.addVertex({ -h.x, -h.y, -h.z }, {  0,  0, -1 }, { 0, 0 }),
    m.addVertex({ -h.x,  h.y, -h.z }, {  0,  0, -1 }, { 0, 1 }),
    m.addVertex({  h.x,  h.y, -h.z }, {  0,  0, -1 }, { 1, 1 }));

  return m;
}

SimpleMesh SimpleMesh::cylinder(double radiusBottom, double radiusTop, double height, bool capBottom, bool capTop, int uSegmentCount, int vSegmentCount)
{
  SimpleMesh m;

  double coneAngle = std::atan2(radiusBottom - radiusTop, height); // Slope
  Vector2d nCone(std::cos(coneAngle), std::sin(coneAngle));

  for (int iu = 0; iu <= uSegmentCount; iu++) {
    double u = iu / double(uSegmentCount); // [0:1]
    double circleAngle = (u * Math::twoPi); // [0:2*pi]
    Vector2d dir(std::cos(circleAngle), std::sin(circleAngle));
    Vector3d n(dir * nCone.x, nCone.y); // Normal

    for (int iv = 0; iv <= vSegmentCount; iv++) {
      double v = iv / double(vSegmentCount); // [0:1]
      double radius = Math::lerp(radiusBottom, radiusTop, v); // [radiusBottom:radiusTop]

      int iVert = m.addVertex({ dir * radius, v * height }, n, { u, v });

      if (iu > 0 && iv > 0) {
        m.addQuad(iVert - 1, iVert, iVert - vSegmentCount - 1, iVert - vSegmentCount - 2);
      }
    }
  }

  if (capBottom) {
    SimpleMesh mDisk = disk(radiusBottom, uSegmentCount);
    mDisk.transform(Matrix44d::rotate(Math::pi, { 1, 0, 0 }));
    m.addMesh(mDisk);
  }

  if (capTop) {
    SimpleMesh mDisk = disk(radiusTop, uSegmentCount);
    mDisk.transform(Matrix44d::translate({ 0, 0, height }));
    m.addMesh(mDisk);
  }

  return m;
}

SimpleMesh SimpleMesh::cylinder(double radius, double height, bool capBottom, bool capTop, int uSegmentCount)
{
  return cylinder(radius, radius, height, capBottom, capTop, uSegmentCount, 1);
}

SimpleMesh SimpleMesh::cone(double radiusBottom, double height, bool capBottom, int uSegmentCount)
{
  SimpleMesh m;

  int iVertCenter = m.addVertex({ 0, 0, height }, { 0, 0, 1 }, { 0.5, 1 });

  double coneAngle = std::atan2(radiusBottom, height); // Slope
  Vector2d nCone(std::cos(coneAngle), std::sin(coneAngle));

  for (int iu = 0; iu <= uSegmentCount; iu++) {
    double u = iu / double(uSegmentCount); // [0:1]
    double circleAngle = (u * Math::twoPi); // [0:2*pi]
    Vector2d dir(std::cos(circleAngle), std::sin(circleAngle));
    Vector3d n(dir * nCone.x, nCone.y);

    int iVert = m.addVertex({ dir * radiusBottom, 0 }, n, { u, 0 });

    if (iu > 0) {
      m.addTriangle(iVert, iVertCenter, iVert - 1);
    }
  }

  if (capBottom) {
    SimpleMesh mDisk = disk(radiusBottom, uSegmentCount);
    mDisk.transform(Matrix44d::rotate(Math::pi, { 1, 0, 0 }));
    m.addMesh(mDisk);
  }

  return m;
}

SimpleMesh SimpleMesh::disk(double radius, int uSegmentCount)
{
  SimpleMesh m;

  int iVertCenter = m.addVertex({ 0, 0, 0 }, { 0, 0, 1 }, { 0.5, 0.5 });

  for (int iu = 0; iu <= uSegmentCount; iu++) {
    double u = iu / double(uSegmentCount); // [0:1]
    double circleAngle = (u * Math::twoPi); // [0:2*pi]
    Vector2d dir(std::cos(circleAngle), std::sin(circleAngle));

    int iVert = m.addVertex({ dir * radius, 0 }, { 0, 0, 1 }, dir * 0.5 + 0.5);

    if (iu > 0) {
      m.addTriangle(iVert, iVertCenter, iVert - 1);
    }
  }

  return m;
}

SimpleMesh SimpleMesh::sphere(double radius, int uSegmentCount, int vSegmentCount)
{
  SimpleMesh m;

  for (int iu = 0; iu <= uSegmentCount; iu++) {
    const double u = iu / double(uSegmentCount); // [0:1]
    const double theta = u * 2.0 * Math::pi; // Azimuthal angle [0:2*pi]

    for (int iv = 0; iv <= vSegmentCount; iv++) {
      const double v = iv / double(vSegmentCount); // [0:1]
      const double phi = (1.0 - v) * Math::pi; // Polar angle [pi:0]

      Vector3d n;
      n.x = std::cos(theta) * std::sin(phi);
      n.y = std::sin(theta) * std::sin(phi);
      n.z = std::cos(phi);

      m.addVertex(n * radius, n, { u, v });

      if (iu > 0 && iv > 0) {
        assert(m.getVertexCount() >= 4);

        SimpleMesh::Quad q;
        q[0] = m.getVertexCount() - 1;
        q[1] = q[0] - (vSegmentCount + 1);
        q[2] = q[0] - (vSegmentCount + 2);
        q[3] = q[0] - 1;

        m.addQuad(q);
      }
    }
  }

  return m;
}

SimpleMesh SimpleMesh::torus(double circleRadius, double tubeRadius, int uSegmentCount, int vSegmentCount)
{
  SimpleMesh m;

  for (int iu = 0; iu <= uSegmentCount; iu++) {
    const double u = iu / double(uSegmentCount); // [0,1]
    const double theta = u * 2.0 * Math::pi; // [0,2*pi]

    const Matrix33d rotate = Matrix33d::rotate(theta, { 0, 0, 1 });
    const Vector3d radCenter = rotate * Vector3d(circleRadius, 0, 0);

    for (int iv = 0; iv <= vSegmentCount; iv++) {
      const double v = iv / double(vSegmentCount); // [0,1]
      const double phi = v * 2.0 * Math::pi; // [0,2*pi]

      const Vector3d vertNormal = rotate * Vector3d(std::cos(phi), 0.0, std::sin(-phi));
      const Vector3d vertPosition = (radCenter + vertNormal * tubeRadius);
      m.addVertex(vertPosition, vertNormal, { u, v });

      if (iu > 0 && iv > 0) {
        assert(m.getVertexCount() >= 4);

        SimpleMesh::Quad q;
        q[0] = m.getVertexCount() - 1;
        q[1] = q[0] - 1;
        q[2] = q[0] - (vSegmentCount + 2);
        q[3] = q[0] - (vSegmentCount + 1);

        m.addQuad(q);
      }
    }
  }

  return m;
}

void SimpleMesh::addMesh(const SimpleMesh &otherMesh)
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

void SimpleMesh::triangulateQuads()
{
  for (const Quad &q : quads) {
    addTriangle(q[0], q[1], q[2]);
    addTriangle(q[0], q[2], q[3]);
  }

  quads.clear();
}

void SimpleMesh::recalculateVertexNormals()
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
        + (v2.pos - v1.pos).cross(v0.pos - v1.pos)
        + (v3.pos - v2.pos).cross(v1.pos - v2.pos)
        + (v0.pos - v3.pos).cross(v2.pos - v3.pos)).normalized();

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

void SimpleMesh::transform(const Matrix44d &m)
{
  const Matrix33d nm = m.get3x3(); // Normal matrix

  for (Vertex &vert : verts) {
    vert.pos = (m * Vector4d(vert.pos, 1.0)).xyz;
    vert.normal = nm * vert.normal;
  }
}

RenderData SimpleMesh::getRenderData() const
{
  RenderData r(RenderData::Mode::TRIANGLES);
  r.addAttr(RenderData::Attribute::POSITION, sizeof(Vector3f));
  r.addAttr(RenderData::Attribute::NORMAL, sizeof(Vector3f));
  r.addAttr(RenderData::Attribute::TEXCOORD, sizeof(Vector2f));
  r.addAttr(RenderData::Attribute::COLOR, sizeof(Vector4b));
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

RenderData SimpleMesh::getRenderDataWireframe() const
{
  RenderData r(RenderData::Mode::LINES);
  r.addAttr(RenderData::Attribute::POSITION, sizeof(Vector3f));
  r.addAttr(RenderData::Attribute::COLOR, sizeof(Vector4b));
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

}
