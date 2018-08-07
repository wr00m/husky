#pragma once

#include <husky/math/Matrix44.hpp>
#include <husky/render/RenderData.hpp>
#include <map>
#include <vector>

namespace husky {

class HUSKY_DLL Bone
{
public:
  Bone();
  Bone(const std::string &name);
  Bone(const std::string &name, const Matrix44d &mtxMeshToBone);

  std::string name;
  Matrix44d mtxMeshToBone;
  //std::map<int, double> vertexWeights;
};

class HUSKY_DLL BoneWeight
{
public:
  BoneWeight();
  BoneWeight(int boneIndex, double weight);

  int boneIndex;
  double weight;
};

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
  typedef Vector2i Line;
  typedef Vector3i Triangle;
  typedef Vector4i Quad;

  int numVerts() const;
  int numTriangles() const;
  int numQuads() const;
  int numBones() const;
  int addVert(const Vector3d &pos);
  int addVert(const Vector3d &pos, const Vector3d &nor, const Vector2d &texCoord);
  void addLine(const Line &l);
  void addLine(int v0, int v1);
  const Line& addLine(const Position &p0, const Position &p1);
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
  bool hasLines() const;
  bool hasFaces() const;
  const std::vector<Position>& getPositions() const;
  Position  getPosition(int iVert) const;
  Normal    getNormal(int iVert) const;
  Tangent   getTangent(int iVert) const;
  TexCoord  getTexCoord(int iVert) const;
  Color     getColor(int iVert) const;
  void setPosition(int iVert, const Position &pos);
  void setNormal(int iVert, const Normal &nor);
  void setTangent(int iVert, const Tangent &tangent);
  void setTexCoord(int iVert, const TexCoord &texCoord);
  void setColor(int iVert, const Color &color);
  void setBoneWeights(int iVert, const std::vector<BoneWeight> &weights);
  void addBoneWeight(int iVert, const BoneWeight &weight);
  const Line& getLine(int iLine) const;
  const Triangle& getTriangle(int iTri) const;
  const Quad& getQuad(int iQuad) const;
  void setAllColors(const Color &color);
  void addMesh(const Mesh &otherMesh);
  void triangulateQuads();
  void recalculateVertexNormals();
  void normalizeBoneWeights();
  void translate(const Vector3d &delta);
  void transform(const Matrix44d &m);
  void convertFacesToWireframeLines();
  RenderData getRenderData() const;

private:
  std::vector<Position> vertPosition;
  std::vector<Normal>   vertNormal;
  std::vector<Tangent>  vertTangent;
  std::vector<TexCoord> vertTexCoord;
  std::vector<Color>    vertColor;
  std::vector<std::vector<BoneWeight>> vertBoneWeights;
  std::vector<Line>     lines;
  std::vector<Triangle> tris;
  std::vector<Quad>     quads;
  std::vector<Bone>     bones;
};

}
