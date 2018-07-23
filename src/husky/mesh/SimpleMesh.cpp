#include <husky/mesh/SimpleMesh.hpp>
#include <husky/math/Math.hpp>

namespace husky {

SimpleMesh SimpleMesh::box(double sizeX, double sizeY, double sizeZ)
{
  const Vector3d h(sizeX * 0.5, sizeY * 0.5, sizeZ * 0.5); // Half size

  SimpleMesh m;

  m.addQuad( // X+
    m.addVert({  h.x, -h.y, -h.z }, {  1,  0,  0 }, { 0, 0 }),
    m.addVert({  h.x,  h.y, -h.z }, {  1,  0,  0 }, { 1, 0 }),
    m.addVert({  h.x,  h.y,  h.z }, {  1,  0,  0 }, { 1, 1 }),
    m.addVert({  h.x, -h.y,  h.z }, {  1,  0,  0 }, { 0, 1 }));

  m.addQuad( // X-
    m.addVert({ -h.x,  h.y, -h.z }, { -1,  0,  0 }, { 0, 0 }),
    m.addVert({ -h.x, -h.y, -h.z }, { -1,  0,  0 }, { 1, 0 }),
    m.addVert({ -h.x, -h.y,  h.z }, { -1,  0,  0 }, { 1, 1 }),
    m.addVert({ -h.x,  h.y,  h.z }, { -1,  0,  0 }, { 0, 1 }));

  m.addQuad( // Y+
    m.addVert({  h.x,  h.y, -h.z }, {  0,  1,  0 }, { 0, 0 }),
    m.addVert({ -h.x,  h.y, -h.z }, {  0,  1,  0 }, { 1, 0 }),
    m.addVert({ -h.x,  h.y,  h.z }, {  0,  1,  0 }, { 1, 1 }),
    m.addVert({  h.x,  h.y,  h.z }, {  0,  1,  0 }, { 0, 1 }));

  m.addQuad( // Y-
    m.addVert({ -h.x, -h.y, -h.z }, {  0, -1,  0 }, { 0, 0 }),
    m.addVert({  h.x, -h.y, -h.z }, {  0, -1,  0 }, { 1, 0 }),
    m.addVert({  h.x, -h.y,  h.z }, {  0, -1,  0 }, { 1, 1 }),
    m.addVert({ -h.x, -h.y,  h.z }, {  0, -1,  0 }, { 0, 1 }));

  m.addQuad( // Z+
    m.addVert({ -h.x, -h.y,  h.z }, {  0,  0,  1 }, { 0, 0 }),
    m.addVert({  h.x, -h.y,  h.z }, {  0,  0,  1 }, { 1, 0 }),
    m.addVert({  h.x,  h.y,  h.z }, {  0,  0,  1 }, { 1, 1 }),
    m.addVert({ -h.x,  h.y,  h.z }, {  0,  0,  1 }, { 0, 1 }));

  m.addQuad( // Z-
    m.addVert({  h.x, -h.y, -h.z }, {  0,  0, -1 }, { 1, 0 }),
    m.addVert({ -h.x, -h.y, -h.z }, {  0,  0, -1 }, { 0, 0 }),
    m.addVert({ -h.x,  h.y, -h.z }, {  0,  0, -1 }, { 0, 1 }),
    m.addVert({  h.x,  h.y, -h.z }, {  0,  0, -1 }, { 1, 1 }));

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

      int iVert = m.addVert({ dir * radius, v * height }, n, { u, v });

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

  int iCenterVert = m.addVert({ 0, 0, height }, { 0, 0, 1 }, { 0.5, 1 });

  double coneAngle = std::atan2(radiusBottom, height); // Slope
  Vector2d nCone(std::cos(coneAngle), std::sin(coneAngle));

  for (int iu = 0; iu <= uSegmentCount; iu++) {
    double u = iu / double(uSegmentCount); // [0:1]
    double circleAngle = (u * Math::twoPi); // [0:2*pi]
    Vector2d dir(std::cos(circleAngle), std::sin(circleAngle));
    Vector3d n(dir * nCone.x, nCone.y);

    int iVert = m.addVert({ dir * radiusBottom, 0 }, n, { u, 0 });

    if (iu > 0) {
      m.addTriangle(iVert, iCenterVert, iVert - 1);
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

  int iCenterVert = m.addVert({ 0, 0, 0 }, { 0, 0, 1 }, { 0.5, 0.5 });

  for (int iu = 0; iu <= uSegmentCount; iu++) {
    double u = iu / double(uSegmentCount); // [0:1]
    double circleAngle = (u * Math::twoPi); // [0:2*pi]
    Vector2d dir(std::cos(circleAngle), std::sin(circleAngle));

    int iVert = m.addVert({ dir * radius, 0 }, { 0, 0, 1 }, dir * 0.5 + 0.5);

    if (iu > 0) {
      m.addTriangle(iVert, iCenterVert, iVert - 1);
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

      m.addVert(n * radius, n, { u, v });

      if (iu > 0 && iv > 0) {
        assert(m.numVerts() >= 4);

        SimpleMesh::Quad q;
        q[0] = m.numVerts() - 1;
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
      m.addVert(vertPosition, vertNormal, { u, v });

      if (iu > 0 && iv > 0) {
        assert(m.numVerts() >= 4);

        SimpleMesh::Quad q;
        q[0] = m.numVerts() - 1;
        q[1] = q[0] - 1;
        q[2] = q[0] - (vSegmentCount + 2);
        q[3] = q[0] - (vSegmentCount + 1);

        m.addQuad(q);
      }
    }
  }

  return m;
}

int SimpleMesh::numVerts() const { return int(vertPosition.size()); }
int SimpleMesh::numTriangles() const { return int(tris.size()); }
int SimpleMesh::numQuads() const { return int(quads.size()); }

int SimpleMesh::addVert(const Vector3d &pos)
{
  int iVert = (int)vertPosition.size();
  vertPosition.emplace_back(pos);
  return iVert;
}

int SimpleMesh::addVert(const Vector3d &pos, const Vector3d &nor, const Vector2d &texCoord)
{
  int iVert = addVert(pos);
  setNormal(iVert, nor);
  setTexCoord(iVert, texCoord);
  return iVert;
}

void SimpleMesh::addTriangle(const Triangle &t) { tris.emplace_back(t); }
void SimpleMesh::addTriangle(int v0, int v1, int v2) { addTriangle({ v0, v1, v2 }); }
const SimpleMesh::Triangle& SimpleMesh::addTriangle(const Position &p0, const Position &p1, const Position &p2) { addTriangle({ addVert(p0), addVert(p1), addVert(p2) }); return tris.back(); }
void SimpleMesh::addQuad(const Quad &q) { quads.emplace_back(q); }
void SimpleMesh::addQuad(int v0, int v1, int v2, int v3) { addQuad({ v0, v1, v2, v3 }); }
const SimpleMesh::Quad& SimpleMesh::addQuad(const Position &p0, const Position &p1, const Position &p2, const Position &p3) { addQuad({ addVert(p0), addVert(p1), addVert(p2), addVert(p3) }); return quads.back(); }
bool SimpleMesh::hasNormals() const { return !vertNormal.empty(); }
bool SimpleMesh::hasTangents() const { return !vertTangent.empty(); }
bool SimpleMesh::hasTexCoord() const { return !vertTexCoord.empty(); }
bool SimpleMesh::hasColors() const { return !vertColor.empty(); }
bool SimpleMesh::hasBoneIndices() const { return !vertBoneIndices.empty(); }
bool SimpleMesh::hasBoneWeights() const { return !vertBoneWeights.empty(); }
void SimpleMesh::setPosition(int iVert, const Position &pos) { vertPosition[iVert] = pos; }
void SimpleMesh::setNormal(int iVert, const Normal &nor) { vertNormal.resize(vertPosition.size()); vertNormal[iVert] = nor; }
void SimpleMesh::setTangent(int iVert, const Tangent &tangent) { vertTangent.resize(vertPosition.size()); vertTangent[iVert] = tangent; }
void SimpleMesh::setTexCoord(int iVert, const TexCoord &texCoord) { vertTexCoord.resize(vertPosition.size()); vertTexCoord[iVert] = texCoord; }
void SimpleMesh::setColor(int iVert, const Color &color) { vertColor.resize(vertPosition.size(), Color(255)); vertColor[iVert] = color; }
void SimpleMesh::setBoneIndices(int iVert, const BoneIndices &inds) { vertBoneIndices.resize(vertPosition.size()); vertBoneIndices[iVert] = inds; }
void SimpleMesh::setBoneWeights(int iVert, const BoneWeights &weights) { vertBoneWeights.resize(vertPosition.size()); vertBoneWeights[iVert] = weights; }
const SimpleMesh::Triangle& SimpleMesh::getTriangle(int iTri) const { return tris[iTri]; }
const SimpleMesh::Quad& SimpleMesh::getQuad(int iQuad) const { return quads[iQuad]; }
//void SimpleMesh::reserve(int vertexCount, int triangleCount, int quadCount) { verts.reserve(vertexCount); tris.reserve(triangleCount); quads.reserve(quadCount); }
void SimpleMesh::setAllColors(const Color &color) { vertColor.assign(vertPosition.size(), color); }

void SimpleMesh::addMesh(const SimpleMesh &m)
{
  //reserve(numVerts() + m.numVerts(), getTriangleCount() + m.getTriangleCount(), getQuadCount() + m.getQuadCount());

  const int indexOffset = numVerts();

  for (int i = 0; i < m.numVerts(); i++) {
    int iVert = addVert(m.vertPosition[i]);
    if (m.hasNormals()) { setNormal(iVert, m.vertNormal[i]); }
    if (m.hasTangents()) { setTangent(iVert, m.vertTangent[i]); }
    if (m.hasTexCoord()) { setTexCoord(iVert, m.vertTexCoord[i]); }
    if (m.hasColors()) { setColor(iVert, m.vertColor[i]); }
    if (m.hasBoneIndices()) { setBoneIndices(iVert, m.vertBoneIndices[i]); }
    if (m.hasBoneWeights()) { setBoneWeights(iVert, m.vertBoneWeights[i]); }
  }

  for (const Triangle &tri : m.tris) {
    tris.emplace_back(tri + indexOffset);
  }

  for (const Quad &quad : m.quads) {
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
  vertNormal.assign(vertPosition.size(), { 0, 0, 0 });

  // Get vertex normal contributions from triangles
  for (const Triangle &tri : tris) {
    const Position &v0 = vertPosition[tri[0]];
    const Position &v1 = vertPosition[tri[1]];
    const Position &v2 = vertPosition[tri[2]];

    const Normal triNormal = (v1- v0).cross(v2- v0).normalized();

    vertNormal[tri[0]] += triNormal;
    vertNormal[tri[1]] += triNormal;
    vertNormal[tri[2]] += triNormal;
  }

  // Get vertex normal contributions from quads
  for (const Quad &quad : quads) {
    const Position &v0 = vertPosition[quad[0]];
    const Position &v1 = vertPosition[quad[1]];
    const Position &v2 = vertPosition[quad[2]];
    const Position &v3 = vertPosition[quad[3]];

    const Normal quadNormal = ((v1 - v0).cross(v3 - v0)
                            +  (v2 - v1).cross(v0 - v1)
                            +  (v3 - v2).cross(v1 - v2)
                            +  (v0 - v3).cross(v2 - v3)).normalized();

    vertNormal[quad[0]] += quadNormal;
    vertNormal[quad[1]] += quadNormal;
    vertNormal[quad[2]] += quadNormal;
    vertNormal[quad[3]] += quadNormal;
  }

  // Normalize vertex normals
  for (Normal &normal : vertNormal) {
    normal.normalize();
  }
}

void SimpleMesh::transform(const Matrix44d &m)
{
  const Matrix33d nm = m.get3x3(); // Normal matrix

  for (int i = 0; i < numVerts(); i++) {
    vertPosition[i] = (m * Vector4d(vertPosition[i], 1.0)).xyz;
    if (hasNormals()) { vertNormal[i] = nm * vertNormal[i]; }
  }
}

RenderData SimpleMesh::getRenderData() const
{
  RenderData r(RenderData::Mode::TRIANGLES);
  r.addAttr(RenderData::Attribute::POSITION, sizeof(Vector3f));
  r.addAttr(RenderData::Attribute::NORMAL, sizeof(Vector3f));
  r.addAttr(RenderData::Attribute::TEXCOORD, sizeof(Vector2f));
  r.addAttr(RenderData::Attribute::COLOR, sizeof(Vector4b));
  r.init(numVerts());

  if (numVerts() > 0) {
    r.anchor = Vector3f(0, 0, 0); //(Vector3f)verts.front().pos;

    for (int i = 0; i < numVerts(); i++) {
      r.setValue(i, RenderData::Attribute::POSITION, (Vector3f)vertPosition[i]);
      if (hasNormals()) { r.setValue(i, RenderData::Attribute::NORMAL, (Vector3f)vertNormal[i]); }
      if (hasTexCoord()) { r.setValue(i, RenderData::Attribute::TEXCOORD, (Vector2f)vertTexCoord[i]); }
      r.setValue(i, RenderData::Attribute::COLOR, hasColors() ? vertColor[i] : Vector4b(255));
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
  r.init(numVerts());

  if (numVerts() > 0) {
    r.anchor = Vector3f(0, 0, 0); //(Vector3f)verts.front().pos;

    for (int i = 0; i < numVerts(); i++) {
      r.setValue(i, RenderData::Attribute::POSITION, (Vector3f)vertPosition[i]);
      r.setValue(i, RenderData::Attribute::COLOR, hasColors() ? vertColor[i] : Vector4b(255));
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
