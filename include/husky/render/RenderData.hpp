#pragma once

#include <husky/math/Matrix44.hpp>
#include <vector>
#include <cassert>

namespace husky {

class Shader;
class Material;
class Viewport;

enum class VertexAttributeDataType
{
  UNDEFINED,
  FLOAT32,
  FLOAT64,
  INT8,
  INT16,
  INT32,
  UINT8,
  UINT16,
  UINT32,
};

class HUSKY_DLL VertexAttribute
{
public:
  static int getBytesPerElement(VertexAttributeDataType dataType);
  static const VertexAttribute Empty;
  static constexpr char POSITION[]      = "Position";
  static constexpr char NORMAL[]        = "Normal";
  static constexpr char TEXCOORD[]      = "TexCoord";
  static constexpr char COLOR[]         = "Color";
  static constexpr char BONE_INDICES[]  = "BoneIndices";
  static constexpr char BONE_WEIGHTS[]  = "BoneWeights";
  static constexpr char TANGENTS[]      = "Tangents";
  // TODO: Pack vertex attributes according to http://www.humus.name/Articles/Persson_CreatingVastGameWorlds.pdf#page=22 / https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices

  VertexAttribute(const std::string &name, VertexAttributeDataType dataType, int elementCount, int byteOffset);

  std::string name;
  VertexAttributeDataType dataType;
  int elementCount;
  int byteCount;
  int byteOffset;
};

class HUSKY_DLL VertexDescription
{
public:
  VertexDescription();

  int addAttr(const std::string &name, VertexAttributeDataType dataType, int elementCount);
  int getAttrIndex(const std::string &name) const;
  const VertexAttribute& getAttr(const std::string &attrName) const;
  const VertexAttribute& getAttr(int i) const;
  inline int getByteCount() const { return byteCount; }

private:
  int byteCount;
  std::vector<VertexAttribute> attrs;
};

enum class PrimitiveType { UNDEFINED, POINTS, LINES, TRIANGLES, };

class HUSKY_DLL RenderData
{
public:
  VertexDescription vertDesc;
  PrimitiveType primitiveType;
  Vector3f anchor;
  int vertCount;
  std::vector<std::uint8_t> bytes;
  std::vector<std::uint16_t> indices;
  unsigned int vbo = 0; // TODO: Remove?
  unsigned int vao = 0; // TODO: Remove?

  RenderData();
  RenderData(const VertexDescription &vertDesc, PrimitiveType primitiveType, int vertCount);
  //~RenderData(); // TODO: Cleanup (VBO, VAO, ...) here?

  void addPoint(int v0);
  void addLine(int v0, int v1);
  void addTriangle(int v0, int v1, int v2);
  bool getAttribPointer(const std::string &attrName, const void *&ptr) const;
  void uploadToGpu(); // TODO: Remove?

  template<typename T>
  bool setValue(int vertIndex, int attrIndex, const T &value)
  {
    const VertexAttribute attr = vertDesc.getAttr(attrIndex);
    if (attr.byteOffset < 0 || vertIndex >= vertCount) {
      assert(0); // We should never get here
      return false;
    }

    assert(sizeof(T) == attr.byteCount);

    const std::uint8_t *b = reinterpret_cast<const std::uint8_t*>(&value);
    
    const int byteStartIndex = (vertIndex * vertDesc.getByteCount() + attr.byteOffset);
    std::copy(b, b + sizeof(T), bytes.begin() + byteStartIndex);

    return true;
  }

  template<typename T>
  T getValue(int vertIndex, int attrIndex) const
  {
    const VertexAttribute attr = vertDesc.getAttr(attrIndex);
    if (attr.byteOffset < 0 || vertIndex >= vertCount) {
      assert(0); // We should never get here
      return T();
    }

    assert(sizeof(T) == attr.byteCount);

    const int byteStartIndex = (vertIndex * vertDesc.getByteCount() + attr.byteOffset);
    const T *t = reinterpret_cast<const T*>(&bytes[byteStartIndex]);

    return *t;
  }

  void draw(const Shader &shader, const Material &mtl, const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection, const std::vector<Matrix44f> &mtxBones = {}) const;
};

}
