#pragma once

#include <husky/math/Matrix44.hpp>
#include <husky/mesh/Bone.hpp>
#include <husky/render/RenderData.hpp>
#include <vector>

namespace husky {

class HUSKY_DLL Mesh
{
public:
  static Mesh box(double sizeX, double sizeY, double sizeZ);
  static Mesh cylinder(double radiusBottom, double radiusTop, double height, bool capBottom, bool capTop, int uSegmentCount, int vSegmentCount);
  static Mesh cylinder(double radius, double height, bool capBottom = true, bool capTop = true, int uSegmentCount = 16);
  static Mesh cone(double radiusBottom, double height, bool capBottom = true, int uSegmentCount = 16);
  static Mesh disk(double radius, int uSegmentCount = 16);
  static Mesh sphere(double radius, int uSegmentCount = 32, int vSegmentCount = 16);
  static Mesh torus(double circleRadius, double tubeRadius, int uSegmentCount = 32, int vSegmentCount = 16);
  //static Mesh capsule();

  typedef Vector3d Position;
  typedef Vector3d Normal;
  typedef Vector3d Tangent;
  typedef Vector2d TexCoord;
  typedef Vector4b Color;
  //typedef std::vector<int> LineStrip;
  typedef Vector3i Triangle;
  typedef Vector4i Quad;

  struct BoneWeight
  {
    int boneIndex;
    double weight;
  };

  int numVerts() const;
  int numTriangles() const;
  int numQuads() const;
  int numBones() const;
  int addVert(const Vector3d &pos);
  int addVert(const Vector3d &pos, const Vector3d &nor, const Vector2d &texCoord);
  void addTriangle(const Triangle &t);
  void addTriangle(int v0, int v1, int v2);
  const Triangle& addTriangle(const Position &p0, const Position &p1, const Position &p2);
  void addQuad(const Quad &q);
  void addQuad(int v0, int v1, int v2, int v3);
  const Quad& addQuad(const Position &p0, const Position &p1, const Position &p2, const Position &p3);
  int addBone(const Bone &bone);
  bool hasNormals() const;
  bool hasTangents() const;
  bool hasTexCoord() const;
  bool hasColors() const;
  bool hasBoneWeights() const;
  void setPosition(int iVert, const Position &pos);
  void setNormal(int iVert, const Normal &nor);
  void setTangent(int iVert, const Tangent &tangent);
  void setTexCoord(int iVert, const TexCoord &texCoord);
  void setColor(int iVert, const Color &color);
  void setBoneWeights(int iVert, const std::vector<BoneWeight> &weights);
  void addBoneWeight(int iVert, const BoneWeight &weight);
  const Triangle& getTriangle(int iTri) const;
  const Quad& getQuad(int iQuad) const;
  void setAllColors(const Color &color);
  void addMesh(const Mesh &otherMesh);
  void triangulateQuads();
  void recalculateVertexNormals();
  void normalizeBoneWeights();
  void transform(const Matrix44d &m);
  RenderData getRenderData() const;
  RenderData getRenderDataWireframe() const;

private:
  std::vector<Vector3d> vertPosition;
  std::vector<Vector3d> vertNormal;
  std::vector<Vector3d> vertTangent;
  std::vector<Vector2d> vertTexCoord;
  std::vector<Vector4b> vertColor;
  std::vector<std::vector<BoneWeight>> vertBoneWeights;
  //std::vector<LineStrip> lineStrips;
  std::vector<Vector3i> tris;
  std::vector<Vector4i> quads;
  std::vector<Bone> bones;
};

}
