#include <husky/mesh/Mesh.hpp>
#include <husky/math/Math.hpp>
#include <husky/Log.hpp>
#include <set>

namespace husky {

Bone::Bone()
  : Bone("")
{
}

Bone::Bone(const std::string &name)
  : Bone(name, Matrix44d::identity())
{
}

Bone::Bone(const std::string &name, const Matrix44d &mtxMeshToBone)
  : name(name)
  , mtxMeshToBone(mtxMeshToBone)
{
}

BoneWeight::BoneWeight()
  : boneIndex(0)
  , weight(0)
{
}

BoneWeight::BoneWeight(int boneIndex, double weight)
  : boneIndex(boneIndex)
  , weight(weight)
{
}

Mesh Mesh::box(double sizeX, double sizeY, double sizeZ)
{
  const Vector3d h(sizeX * 0.5, sizeY * 0.5, sizeZ * 0.5); // Half size

  Mesh m;

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

Mesh Mesh::cylinder(double radiusBottom, double radiusTop, double height, bool capBottom, bool capTop, int uSegmentCount, int vSegmentCount)
{
  Mesh m;

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
    Mesh mDisk = disk(radiusBottom, uSegmentCount);
    mDisk.transform(Matrix44d::rotate(Math::pi, { 1, 0, 0 }));
    m.addMesh(mDisk);
  }

  if (capTop) {
    Mesh mDisk = disk(radiusTop, uSegmentCount);
    mDisk.transform(Matrix44d::translate({ 0, 0, height }));
    m.addMesh(mDisk);
  }

  return m;
}

Mesh Mesh::cylinder(double radius, double height, bool capBottom, bool capTop, int uSegmentCount)
{
  return cylinder(radius, radius, height, capBottom, capTop, uSegmentCount, 1);
}

Mesh Mesh::cone(double radiusBottom, double height, bool capBottom, int uSegmentCount)
{
  Mesh m;

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
    Mesh mDisk = disk(radiusBottom, uSegmentCount);
    mDisk.transform(Matrix44d::rotate(Math::pi, { 1, 0, 0 }));
    m.addMesh(mDisk);
  }

  return m;
}

Mesh Mesh::disk(double radius, int uSegmentCount)
{
  Mesh m;

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

Mesh Mesh::sphere(double radius, int uSegmentCount, int vSegmentCount)
{
  Mesh m;

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

        Mesh::Quad q;
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

Mesh Mesh::torus(double circleRadius, double tubeRadius, int uSegmentCount, int vSegmentCount)
{
  Mesh m;

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

        Mesh::Quad q;
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

Mesh Mesh::axes(double axisLength, int uSegmentCount)
{
  double cylinderLength = axisLength * 0.75;

  Mesh mArrow = cylinder(0.05 * axisLength, cylinderLength, true, false, uSegmentCount);
  Mesh mCone = cone(0.1 * axisLength, axisLength - cylinderLength, true, uSegmentCount);
  mCone.translate({ 0, 0, cylinderLength });
  mArrow.addMesh(mCone);

  // Z axis
  Mesh m;
  mArrow.setAllColors({ 0, 0, 255, 255 });
  m.addMesh(mArrow);

  // Y axis
  mArrow.transform(Matrix44d::rotate(-Math::pi2, { 1, 0, 0 }));
  mArrow.setAllColors({ 0, 255, 0, 255 });
  m.addMesh(mArrow);

  // X axis
  mArrow.transform(Matrix44d::rotate(-Math::pi2, { 0, 0, 1 }));
  mArrow.setAllColors({ 255, 0, 0, 255 });
  m.addMesh(mArrow);

  return m;
}

int Mesh::numVerts() const { return int(vertPosition.size()); }
int Mesh::numTriangles() const { return int(tris.size()); }
int Mesh::numQuads() const { return int(quads.size()); }
int Mesh::numBones() const { return int(bones.size()); }
int Mesh::addVert(const Vector3d &pos) { vertPosition.emplace_back(pos); return int(vertPosition.size() - 1); }
int Mesh::addVert(const Vector3d &pos, const Vector3d &nor, const Vector2d &texCoord) { int iVert = addVert(pos); setNormal(iVert, nor); setTexCoord(iVert, texCoord); return iVert; }
void Mesh::addLine(const Line &l) { lines.emplace_back(l); }
void Mesh::addLine(int v0, int v1) { addLine({ v0, v1 }); }
const Mesh::Line& Mesh::addLine(const Position &p0, const Position &p1) { addLine({ addVert(p0), addVert(p1) }); return lines.back(); }
void Mesh::addTriangle(const Triangle &t) { tris.emplace_back(t); }
void Mesh::addTriangle(int v0, int v1, int v2) { addTriangle({ v0, v1, v2 }); }
const Mesh::Triangle& Mesh::addTriangle(const Position &p0, const Position &p1, const Position &p2) { addTriangle({ addVert(p0), addVert(p1), addVert(p2) }); return tris.back(); }
void Mesh::addQuad(const Quad &q) { quads.emplace_back(q); }
void Mesh::addQuad(int v0, int v1, int v2, int v3) { addQuad({ v0, v1, v2, v3 }); }
const Mesh::Quad& Mesh::addQuad(const Position &p0, const Position &p1, const Position &p2, const Position &p3) { addQuad({ addVert(p0), addVert(p1), addVert(p2), addVert(p3) }); return quads.back(); }
int Mesh::addBone(const Bone &bone) { bones.emplace_back(bone); return int(bones.size() - 1); }
bool Mesh::hasNormals() const { return !vertNormal.empty(); }
bool Mesh::hasTangents() const { return !vertTangent.empty(); }
bool Mesh::hasTexCoord() const { return !vertTexCoord.empty(); }
bool Mesh::hasColors() const { return !vertColor.empty(); }
bool Mesh::hasBoneWeights() const { return !vertBoneWeights.empty(); }
bool Mesh::hasLines() const { return !lines.empty(); }
bool Mesh::hasFaces() const { return !tris.empty() || !quads.empty(); }
bool Mesh::hasBones() const { return !bones.empty(); }
const std::vector<Mesh::Position>& Mesh::getPositions() const { return vertPosition; }
const std::vector<Bone>& Mesh::getBones() const { return bones; }
Mesh::Position Mesh::getPosition(int iVert) const { return vertPosition[iVert]; }
Mesh::Normal Mesh::getNormal(int iVert) const { return vertNormal[iVert]; }
Mesh::Tangent Mesh::getTangent(int iVert) const { return vertTangent[iVert]; }
Mesh::TexCoord Mesh::getTexCoord(int iVert) const { return vertTexCoord[iVert]; }
Mesh::Color Mesh::getColor(int iVert) const { return vertColor[iVert]; }
void Mesh::setPosition(int iVert, const Position &pos) { vertPosition[iVert] = pos; }
void Mesh::setNormal(int iVert, const Normal &nor) { vertNormal.resize(vertPosition.size()); vertNormal[iVert] = nor; }
void Mesh::setTangent(int iVert, const Tangent &tangent) { vertTangent.resize(vertPosition.size()); vertTangent[iVert] = tangent; }
void Mesh::setTexCoord(int iVert, const TexCoord &texCoord) { vertTexCoord.resize(vertPosition.size()); vertTexCoord[iVert] = texCoord; }
void Mesh::setColor(int iVert, const Color &color) { vertColor.resize(vertPosition.size(), Color(255)); vertColor[iVert] = color; }
void Mesh::setBoneWeights(int iVert, const std::vector<BoneWeight> &weights) { vertBoneWeights.resize(vertPosition.size()); vertBoneWeights[iVert] = weights; }
void Mesh::addBoneWeight(int iVert, const BoneWeight &weight) { vertBoneWeights.resize(vertPosition.size()); vertBoneWeights[iVert].emplace_back(weight); }
const Mesh::Line& Mesh::getLine(int iLine) const { return lines[iLine]; }
const Mesh::Triangle& Mesh::getTriangle(int iTri) const { return tris[iTri]; }
const Mesh::Quad& Mesh::getQuad(int iQuad) const { return quads[iQuad]; }
void Mesh::setAllColors(const Color &color) { vertColor.assign(vertPosition.size(), color); }

void Mesh::addMesh(const Mesh &m)
{
  vertPosition.reserve(numVerts() + m.numVerts());
  if (hasNormals() || m.hasNormals()) { vertNormal.reserve(vertPosition.capacity()); }
  if (hasTangents() || m.hasTangents()) { vertTangent.reserve(vertPosition.capacity()); }
  if (hasTexCoord() || m.hasTexCoord()) { vertTexCoord.reserve(vertPosition.capacity()); }
  if (hasColors() || m.hasColors()) { vertColor.reserve(vertPosition.capacity()); }
  if (hasBoneWeights() || m.hasBoneWeights()) { vertBoneWeights.reserve(vertPosition.capacity()); }
  //lineStrips.reserve(numLineStrips() + m.numLineStrips());
  tris.reserve(numTriangles() + m.numTriangles());
  quads.reserve(numQuads() + m.numQuads());
  bones.reserve(numBones() + m.numBones());

  const int vertOffset = numVerts();
  const int boneOffset = numBones();

  for (int i = 0; i < m.numVerts(); i++) {
    int iVert = addVert(m.vertPosition[i]);
    assert(iVert == i + vertOffset);

    if (m.hasNormals()) { setNormal(iVert, m.vertNormal[i]); }
    if (m.hasTangents()) { setTangent(iVert, m.vertTangent[i]); }
    if (m.hasTexCoord()) { setTexCoord(iVert, m.vertTexCoord[i]); }
    if (m.hasColors()) { setColor(iVert, m.vertColor[i]); }

    if (m.hasBoneWeights()) {
      for (const auto &boneWeights : m.vertBoneWeights) {
        for (const BoneWeight &boneWeight : boneWeights) {
          addBoneWeight(iVert, BoneWeight(boneWeight.boneIndex + boneOffset, boneWeight.weight));
        }
      }
    }
  }

  for (const Triangle &tri : m.tris) {
    addTriangle(tri + vertOffset);
  }

  for (const Quad &quad : m.quads) {
    addQuad(quad + vertOffset);
  }

  for (const Bone &bone : m.bones) {
    addBone(bone);
  }
}

void Mesh::triangulateQuads()
{
  for (const Quad &q : quads) {
    addTriangle(q[0], q[1], q[2]);
    addTriangle(q[0], q[2], q[3]);
  }

  quads.clear();
}

void Mesh::recalculateVertexNormals()
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

void Mesh::normalizeBoneWeights()
{
  if (!hasBoneWeights()) {
    return;
  }

  for (int i = 0; i < numVerts(); i++) {
    std::vector<BoneWeight> &boneWeights = vertBoneWeights[i];

    if (boneWeights.size() > 4) {
      // TODO: Sort by weight
      boneWeights.resize(4);

      Vector4d normWeights(boneWeights[0].weight, boneWeights[1].weight, boneWeights[2].weight, boneWeights[3].weight);
      normWeights.normalize();

      boneWeights[0].weight = normWeights[0];
      boneWeights[1].weight = normWeights[1];
      boneWeights[2].weight = normWeights[2];
      boneWeights[3].weight = normWeights[3];
    }
  }
}

void Mesh::translate(const Vector3d &delta) // More efficient than transform()
{
  for (Position &pos : vertPosition) {
    pos += delta;
  }
}

void Mesh::transform(const Matrix44d &m)
{
  if (numVerts() > 0) {
    const Matrix33d nm = m.get3x3(); // Normal matrix

    for (int i = 0; i < numVerts(); i++) {
      vertPosition[i] = (m * Vector4d(vertPosition[i], 1.0)).xyz;
      if (hasNormals()) { vertNormal[i] = nm * vertNormal[i]; }
    }
  }
}

class LineComp
{
public:
  bool operator()(const Mesh::Line &a, const Mesh::Line &b) const
  {
    if (a[0] < b[0]) return true;
    else if (a[0] > b[0]) return false;
    else if (a[1] < b[1]) return true;
    else return false;
  }
};

void Mesh::convertFacesToWireframeLines()
{
  std::set<Line, LineComp> uniqueLines;

  for (const Triangle &t : tris) {
    uniqueLines.emplace(t[0], t[1]);
    uniqueLines.emplace(t[1], t[2]);
    uniqueLines.emplace(t[2], t[0]);
  }
  tris.clear();

  for (const Quad &q : quads) {
    uniqueLines.emplace(q[0], q[1]);
    uniqueLines.emplace(q[1], q[2]);
    uniqueLines.emplace(q[2], q[3]);
    uniqueLines.emplace(q[3], q[0]);
  }
  quads.clear();

  for (const Line &line : uniqueLines) {
    addLine(line);
  }
}

RenderData Mesh::getRenderData() const
{
  if (hasFaces()) {
    if (hasLines()) {
      Log::warning("Mesh has both lines and faces");
    }

    RenderData r(RenderData::Mode::TRIANGLES);
    r.addAttr(RenderData::Attribute::POSITION, sizeof(Vector3f));
    r.addAttr(RenderData::Attribute::NORMAL, sizeof(Vector3f));
    r.addAttr(RenderData::Attribute::TEXCOORD, sizeof(Vector2f));
    r.addAttr(RenderData::Attribute::COLOR, sizeof(Vector4b));
    if (hasBoneWeights()) {
      r.addAttr(RenderData::Attribute::BONE_INDICES, sizeof(Vector4b));
      r.addAttr(RenderData::Attribute::BONE_WEIGHTS, sizeof(Vector4b));
    }
    r.init(numVerts());

    if (numVerts() > 0) {
      r.anchor = Vector3f(0, 0, 0); //(Vector3f)verts.front().pos;

      for (int i = 0; i < numVerts(); i++) {
        r.setValue(i, RenderData::Attribute::POSITION, (Vector3f)vertPosition[i]);
        if (hasNormals()) { r.setValue(i, RenderData::Attribute::NORMAL, (Vector3f)vertNormal[i]); }
        if (hasTexCoord()) { r.setValue(i, RenderData::Attribute::TEXCOORD, (Vector2f)vertTexCoord[i]); }
        r.setValue(i, RenderData::Attribute::COLOR, hasColors() ? vertColor[i] : Vector4b(255));

        if (hasBoneWeights()) {
          const auto &boneWeights = vertBoneWeights[i];
          Vector4b indices; // = { 0, 1, 2, 3 };
          Vector4b weights; // = { 255, 0, 0, 0 };

          for (int j = 0; j < boneWeights.size(); j++) {
            if (j > 3) {
              Log::warning("Too many bone weights, should be normalized");
              break;
            }

            indices[j] = (std::uint8_t)boneWeights[j].boneIndex;
            weights[j] = (std::uint8_t)(boneWeights[j].weight * 255); // Check/clamp value?
          }

          r.setValue(i, RenderData::Attribute::BONE_INDICES, indices);
          r.setValue(i, RenderData::Attribute::BONE_WEIGHTS, weights);
        }
      }

      for (const Triangle &t : tris) {
        r.addTriangle(t[0], t[1], t[2]);
      }

      for (const Quad &q : quads) {
        r.addTriangle(q[0], q[1], q[2]);
        r.addTriangle(q[0], q[2], q[3]);
      }
    }

    r.uploadToGpu();
    return r;
  }
  else if (hasLines()) {
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

      for (const Line &l : lines) {
        r.addLine(l[0], l[1]);
      }

      //for (const Triangle &t : tris) {
      //  r.addLine(t[0], t[1]);
      //  r.addLine(t[1], t[2]);
      //  r.addLine(t[2], t[0]);
      //}

      //for (const Quad &q : quads) {
      //  r.addLine(q[0], q[1]);
      //  r.addLine(q[1], q[2]);
      //  r.addLine(q[2], q[3]);
      //  r.addLine(q[3], q[0]);
      //}
    }

    r.uploadToGpu();
    return r;
  }
  else { // Neither faces nor lines => Assume points
    RenderData r(RenderData::Mode::POINTS);
    r.addAttr(RenderData::Attribute::POSITION, sizeof(Vector3f));
    r.addAttr(RenderData::Attribute::COLOR, sizeof(Vector4b));
    r.init(numVerts());

    if (numVerts() > 0) {
      r.anchor = Vector3f(0, 0, 0); //(Vector3f)verts.front().pos;

      for (int i = 0; i < numVerts(); i++) {
        r.setValue(i, RenderData::Attribute::POSITION, (Vector3f)vertPosition[i]);
        r.setValue(i, RenderData::Attribute::COLOR, hasColors() ? vertColor[i] : Vector4b(255));
        //r.addPoint(i); // TODO: Remove?
      }
    }

    r.uploadToGpu();
    return r;
  }
}

}
