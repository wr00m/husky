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

  static constexpr char POSITION[]      = "vertPosition";
  static constexpr char NORMAL[]        = "vertNormal";
  static constexpr char TEXCOORD[]      = "vertTexCoord";
  static constexpr char COLOR[]         = "vertColor";
  static constexpr char BONE_INDICES[]  = "vertBoneIndices";
  static constexpr char BONE_WEIGHTS[]  = "vertBoneWeights";
  static constexpr char TANGENTS[]      = "vertTangents";
  // TODO: Pack vertex attributes according to http://www.humus.name/Articles/Persson_CreatingVastGameWorlds.pdf#page=22 / https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices

  VertexAttribute(const std::string &name, VertexAttributeDataType dataType, int elementCount, bool normalize, int byteOffset);

  operator bool() const;
  const void* getAttribPointer() const;

  std::string name;
  VertexAttributeDataType dataType;
  int elementCount;
  bool normalize;
  int byteCount;
  int byteOffset;
};

class HUSKY_DLL VertexDescription
{
public:
  VertexDescription();

  int addAttr(const std::string &name, VertexAttributeDataType dataType, int elementCount, bool normalize = false);
  int getAttrIndex(const std::string &name) const;
  const VertexAttribute& getAttr(const std::string &attrName) const;
  const VertexAttribute& getAttr(int i) const;
  //inline int getByteCount() const { return byteCount; }

//private:
  int byteCount;
  std::vector<VertexAttribute> attrs;
};

class HUSKY_DLL VertexData
{
public:
  VertexData(const VertexDescription &vertDesc, int vertCount);

  VertexDescription vertDesc;
  std::vector<std::uint8_t> bytes;
  int vertCount;
  Vector3f anchor;

  template<typename T>
  bool setValue(int vertIndex, int attrIndex, const T &value)
  {
    if (vertIndex < 0 || vertIndex >= vertCount) {
      return false;
    }

    if (const VertexAttribute &attr = vertDesc.getAttr(attrIndex)) {
      if (sizeof(T) > attr.byteCount) {
        return false;
      }

      const std::uint8_t *b = reinterpret_cast<const std::uint8_t*>(&value);
      const int byteStartIndex = (vertIndex * vertDesc.byteCount + attr.byteOffset);
      std::copy(b, b + sizeof(T), bytes.begin() + byteStartIndex);
      return true;
    }

    return false;
  }

  //template<typename T>
  //T getValue(int vertIndex, int attrIndex) const
  //{
  //  if (vertIndex < 0 || vertIndex >= vertCount) {
  //    return T();
  //  }

  //  if (const VertexAttribute &attr = vertDesc.getAttr(attrIndex)) {
  //    if (sizeof(T) < attr.byteCount) {
  //      return T();
  //    }

  //    const int byteStartIndex = (vertIndex * vertDesc.byteCount + attr.byteOffset);
  //    const T *t = reinterpret_cast<const T*>(&bytes[byteStartIndex]);
  //    return *t;
  //  }

  //  return T();
  //}
};

enum class PrimitiveType { UNDEFINED, POINTS, LINES, TRIANGLES, };

class HUSKY_DLL IndexData
{
public:
  IndexData(PrimitiveType primitiveType);

  void addPoint(int v0);
  void addLine(int v0, int v1);
  void addTriangle(int v0, int v1, int v2);

  PrimitiveType primitiveType;
  std::vector<std::uint16_t> indices;
};

class HUSKY_DLL RenderData
{
public:
  unsigned int vbo = 0; // TODO: Remove?
  unsigned int vao = 0; // TODO: Remove?

  RenderData();
  RenderData(VertexData &&vertData, IndexData &&indexData);
  //~RenderData(); // TODO: Cleanup (VBO, VAO, ...) here?

  void uploadToGpu(); // TODO: Remove?
  void draw(const Shader &shader, const Material &mtl, const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection, const std::vector<Matrix44f> &mtxBones = {}) const;
  
  VertexData _vertData;
  IndexData _indexData;
};

}
