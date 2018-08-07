#pragma once

#include <husky/math/Matrix44.hpp>
#include <vector>
#include <cassert>

namespace husky {

class Shader;
class Material;
class Viewport;

class HUSKY_DLL RenderData
{
public:
  enum class Mode { POINTS, LINES, TRIANGLES, };

  enum class Attribute
  {
    POSITION,
    NORMAL,
    TEXCOORD,
    COLOR,
    BONE_INDICES,
    BONE_WEIGHTS,
    //TANGENTS,
    // TODO: Pack vertex attributes according to http://www.humus.name/Articles/Persson_CreatingVastGameWorlds.pdf#page=22 / https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
  };

  Mode mode;
  Vector3f anchor;
  int vertCount;
  int vertByteCount;
  std::vector<std::uint8_t> bytes;
  std::vector<int> attrByteOffsets;
  std::vector<std::uint16_t> indices;
  unsigned int vbo = 0; // TODO: Remove?
  unsigned int vao = 0; // TODO: Remove?

  RenderData();
  RenderData(Mode mode);
  //~RenderData(); // TODO: Cleanup here?

  void init(int vertCount);
  void addPoint(int v0);
  void addLine(int v0, int v1);
  void addTriangle(int v0, int v1, int v2);
  void addAttr(Attribute attr, int attrByteCount);
  bool getAttribPointer(Attribute attr, const void *&ptr) const;
  void uploadToGpu(); // TODO: Remove?

  template<typename T>
  bool setValue(int vertIndex, Attribute attr, const T &value)
  {
    const int attrByteOffset = attrByteOffsets[(int)attr];
    if (attrByteOffset < 0) {
      return false;
    }

    const std::uint8_t *b = reinterpret_cast<const std::uint8_t*>(&value);
    
    const int byteStartIndex = (vertIndex * vertByteCount + attrByteOffset);
    std::copy(b, b + sizeof(T), bytes.begin() + byteStartIndex);

    return true;
  }

  template<typename T>
  T getValue(int vertIndex, Attribute attr) const
  {
    const int attrByteOffset = attrByteOffsets[(int)attr];
    assert(attrByteOffset >= 0);

    const int byteStartIndex = (vertIndex * vertByteCount + attrByteOffset);
    const T *t = reinterpret_cast<const T*>(&bytes[byteStartIndex]);

    return *t;
  }

  void draw(const Shader &shader, const Material &mtl, const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection) const;
};

}
