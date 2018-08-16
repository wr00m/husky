#include <husky/render/RenderData.hpp>
#include <husky/render/Shader.hpp>
#include <husky/render/Viewport.hpp>
#include <husky/mesh/Material.hpp>
#include <husky/Log.hpp>
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

void RenderData::draw(const Shader &shader, const Material &mtl, const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection, const std::vector<Matrix44f> &mtxBones) const
{
  if (shader.shaderProgramHandle == 0) {
    Log::warning("Invalid shader program");
    return;
  }

  if (mtl.twoSided) {
    glDisable(GL_CULL_FACE);
  }
  else {
    glEnable(GL_CULL_FACE);
  }

  if (mtl.depthTest) {
    glEnable(GL_DEPTH_TEST);
  }
  else {
    glDisable(GL_DEPTH_TEST);
  }

  glUseProgram(shader.shaderProgramHandle);

  int varLocation;

  if (shader.getUniformLocation("mtxModelView", varLocation)) {
    glUniformMatrix4fv(varLocation, 1, GL_FALSE, modelView.m);
  }

  const Matrix33f normalMatrix = modelView.get3x3(); // Fast, but only works with uniform scaling
  //const Matrix33f normalMatrix = modelView.inverted().transposed().get3x3(); // TODO: Use pre-inverted matrix for better performance
  //const Matrix33f normalMatrix = modelView.get3x3().inverted().transposed();
  if (shader.getUniformLocation("mtxNormal", varLocation)) {
    glUniformMatrix3fv(varLocation, 1, GL_FALSE, normalMatrix.m);
  }

  if (shader.getUniformLocation("mtxProjection", varLocation)) {
    glUniformMatrix4fv(varLocation, 1, GL_FALSE, projection.m);
  }

  if (shader.getUniformLocation("mtxBones", varLocation)) {
    if (mtxBones.empty()) {
      glUniformMatrix4fv(varLocation, 1, GL_FALSE, Matrix44f::identity().m); // Single identity matrix
    }
    else {
      glUniformMatrix4fv(varLocation, (GLsizei)mtxBones.size(), GL_FALSE, mtxBones.front().m);
    }
  }

  if (shader.getUniformLocation("useBones", varLocation)) {
    glUniform1i(varLocation, !mtxBones.empty());
  }

  if (shader.getUniformLocation("tex", varLocation)) {
    glUniform1i(varLocation, 0);
  }

  if (shader.getUniformLocation("lightDir", varLocation)) {
    Vector3f lightDir(20, -40, 100); // TODO
    lightDir = (view * Vector4f(lightDir, 0.0)).xyz.normalized();
    glUniform3fv(varLocation, 1, lightDir.val);
  }

  if (shader.getUniformLocation("mtlAmbient", varLocation)) {
    glUniform3fv(varLocation, 1, mtl.ambient.val);
  }

  if (shader.getUniformLocation("mtlDiffuse", varLocation)) {
    glUniform3fv(varLocation, 1, mtl.diffuse.val);
  }

  if (shader.getUniformLocation("mtlSpecular", varLocation)) {
    glUniform3fv(varLocation, 1, mtl.specular.val);
  }

  if (shader.getUniformLocation("mtlEmissive", varLocation)) {
    glUniform3fv(varLocation, 1, mtl.emissive.val);
  }

  if (shader.getUniformLocation("mtlShininess", varLocation)) {
    glUniform1f(varLocation, mtl.shininess);
  }

  if (shader.getUniformLocation("mtlShininessStrength", varLocation)) {
    glUniform1f(varLocation, mtl.shininessStrength);
  }

  if (shader.getUniformLocation("viewportSize", varLocation)) {
    glUniform2f(varLocation, (float)viewport.width, (float)viewport.height);
  }

  if (shader.getUniformLocation("lineWidth", varLocation)) {
    glUniform1f(varLocation, mtl.lineWidth);
  }

  if (mtl.tex.valid()) {
    //glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, mtl.tex.handle);
  }
  else {
    glBindTexture(GL_TEXTURE_2D, 0);
    //glDisable(GL_TEXTURE_2D);
  }

  if (vbo == 0) {
    Log::warning("VBO is 0");
    return;
  }

  if (vao == 0) {
    Log::warning("VAO is 0");
    return;
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindVertexArray(vao);

  const void *attrPtr = nullptr;

  if (shader.getAttributeLocation("vertPosition", varLocation) && getAttribPointer(RenderData::Attribute::POSITION, attrPtr)) {
    glEnableVertexAttribArray(varLocation);
    glVertexAttribPointer(varLocation, 3, GL_FLOAT, GL_FALSE, vertByteCount, attrPtr);
  }

  if (shader.getAttributeLocation("vertNormal", varLocation) && getAttribPointer(RenderData::Attribute::NORMAL, attrPtr)) {
    glEnableVertexAttribArray(varLocation);
    glVertexAttribPointer(varLocation, 3, GL_FLOAT, GL_FALSE, vertByteCount, attrPtr);
  }

  if (shader.getAttributeLocation("vertTexCoord", varLocation) && getAttribPointer(RenderData::Attribute::TEXCOORD, attrPtr)) {
    glEnableVertexAttribArray(varLocation);
    glVertexAttribPointer(varLocation, 2, GL_FLOAT, GL_FALSE, vertByteCount, attrPtr);
  }

  if (shader.getAttributeLocation("vertColor", varLocation) && getAttribPointer(RenderData::Attribute::COLOR, attrPtr)) {
    glEnableVertexAttribArray(varLocation);
    glVertexAttribPointer(varLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, vertByteCount, attrPtr);
  }

  if (shader.getAttributeLocation("vertBoneIndices", varLocation) && getAttribPointer(RenderData::Attribute::BONE_INDICES, attrPtr)) {
    glEnableVertexAttribArray(varLocation);
    glVertexAttribIPointer(varLocation, 4, GL_UNSIGNED_BYTE, vertByteCount, attrPtr);
  }
  //else { glDisableVertexAttribArray(varLocation); } // TODO

  if (shader.getAttributeLocation("vertBoneWeights", varLocation) && getAttribPointer(RenderData::Attribute::BONE_WEIGHTS, attrPtr)) {
    glEnableVertexAttribArray(varLocation);
    glVertexAttribPointer(varLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, vertByteCount, attrPtr);
  }

  GLenum mode = GL_POINTS; // Default fallback
  switch (this->mode) {
  case     RenderData::Mode::POINTS:    mode = GL_POINTS;          break;
  case     RenderData::Mode::LINES:     mode = GL_LINES;           break;
  case     RenderData::Mode::TRIANGLES: mode = GL_TRIANGLES;       break;
  default: Log::warning("Unsupported RenderData::Mode: %d", mode); break;
  }

  if (indices.empty()) {
    glDrawArrays(mode, 0, vertCount);
  }
  else {
    glDrawElements(mode, (int)indices.size(), GL_UNSIGNED_SHORT, indices.data());
  }
}

}
