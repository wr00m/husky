#include <husky/render/RenderData.hpp>
#include <glad/glad.h>

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
  , attrByteOffsets((int)Attribute::BONE_WEIGHTS + 1, -1)
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

void RenderData::uploadToGpu()
{
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, bytes.size(), bytes.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao);
  //glBindVertexArray(vao);
}

}
