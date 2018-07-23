#include <husky/render/RenderData.hpp>

namespace husky {

RenderData::RenderData()
  : RenderData(Mode::POINTS)
{
}

RenderData::RenderData(Mode mode)
  : mode(mode)
  , anchor(0, 0, 0)
  , vertCount(0)
  , vertByteCount(0)
  , bytes{}
  , attrByteOffsets((int)Attribute::COLOR + 1, -1)
{
}

void RenderData::init(int vertCount)
{
  this->vertCount = vertCount;
  bytes.resize(vertCount * vertByteCount);
}

void RenderData::addPoint(int v0)
{
  indices.emplace_back(v0);
}

void RenderData::addLine(int v0, int v1)
{
  indices.emplace_back(v0);
  indices.emplace_back(v1);
}

void RenderData::addTriangle(int v0, int v1, int v2)
{
  indices.emplace_back(v0);
  indices.emplace_back(v1);
  indices.emplace_back(v2);
}

void RenderData::addAttr(Attribute attr, int attrByteCount)
{
  attrByteOffsets[(int)attr] = vertByteCount;
  vertByteCount += attrByteCount;
  //return attrByteOffsets[(int)attr];
}

bool RenderData::getAttribPointer(Attribute attr, const void *&ptr) const
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

}
