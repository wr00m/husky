#pragma once

#include <husky/math/Vector3.hpp>
#include <vector>
#include <cassert>

namespace husky {

class RenderData
{
public:
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

  enum class Mode
  {
    POINTS,
    LINES,
    TRIANGLES,
  };

  Mode mode;
  Vector3f anchor;
  int vertCount;
  int vertByteCount;
  std::vector<std::uint8_t> bytes;
  std::vector<int> attrByteOffsets;
  std::vector<std::uint16_t> indices;
  unsigned int vbo = 0; // TODO: Remove
  unsigned int vao = 0; // TODO: Remove

  RenderData()
    : RenderData(Mode::POINTS)
  {
  }

  RenderData(Mode mode)
    : mode(mode)
    , anchor(0, 0, 0)
    , vertCount(0)
    , vertByteCount(0)
    , bytes{}
    , attrByteOffsets((int)Attribute::COLOR + 1, -1)
  {
  }

  void init(int vertCount)
  {
    this->vertCount = vertCount;
    bytes.resize(vertCount * vertByteCount);
  }

  void addPoint(int v0)
  {
    indices.emplace_back(v0);
  }

  void addLine(int v0, int v1)
  {
    indices.emplace_back(v0);
    indices.emplace_back(v1);
  }

  void addTriangle(int v0, int v1, int v2)
  {
    indices.emplace_back(v0);
    indices.emplace_back(v1);
    indices.emplace_back(v2);
  }

  void addAttr(Attribute attr, int attrByteCount)
  {
    attrByteOffsets[(int)attr] = vertByteCount;
    vertByteCount += attrByteCount;
    //return attrByteOffsets[(int)attr];
  }

  bool getAttribPointer(Attribute attr, const void *&ptr) const
  {
    int offset = attrByteOffsets[(int)attr];
    if (offset >= 0) {
      ptr = ((const std::uint8_t*)nullptr) + offset;
      return true;
    }
    else {
      ptr = nullptr;
      return false;
    }
  }

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
};

}
