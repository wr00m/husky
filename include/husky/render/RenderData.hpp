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

  bool hasAttrib(Attribute attr) const
  {
    return (attrByteOffsets[(int)attr] != -1);
  }

  const void* attribPointer(Attribute attr) const
  {
    return ((const std::uint8_t*)NULL) + attrByteOffsets[(int)attr];
  }

  template<typename T>
  void setValue(int vertIndex, Attribute attr, const T &value)
  {
    const int attrByteOffset = attrByteOffsets[(int)attr];
    assert(attrByteOffset >= 0);

    const std::uint8_t *b = reinterpret_cast<const std::uint8_t*>(&value);
    
    const int byteStartIndex = (vertIndex * vertByteCount + attrByteOffset);
    std::copy(b, b + sizeof(T), bytes.begin() + byteStartIndex);
  }
};

}
