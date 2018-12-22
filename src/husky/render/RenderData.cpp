#include <husky/render/RenderData.hpp>
#include <husky/render/Shader.hpp>
#include <husky/render/Viewport.hpp>
#include <husky/mesh/Material.hpp>
#include <husky/Log.hpp>
#include <glad/glad.h>

namespace husky {

int VertexAttribute::getBytesPerElement(VertexAttributeDataType dataType)
{
  switch (dataType)
  {
  case VertexAttributeDataType::FLOAT32 : return 4;
  case VertexAttributeDataType::FLOAT64 : return 8;
  case VertexAttributeDataType::INT8    : return 1;
  case VertexAttributeDataType::INT16   : return 2;
  case VertexAttributeDataType::INT32   : return 4;
  case VertexAttributeDataType::UINT8   : return 1;
  case VertexAttributeDataType::UINT16  : return 2;
  case VertexAttributeDataType::UINT32  : return 4;
  default: return 0;
  }
}

VertexAttribute::VertexAttribute(const std::string &name, VertexAttributeDataType dataType, int elementCount, bool normalize, int byteOffset)
  : name(name)
  , dataType(dataType)
  , elementCount(elementCount)
  , normalize(normalize)
  , byteCount(getBytesPerElement(dataType) * elementCount)
  , byteOffset(byteOffset)
{
}

VertexAttribute::operator bool() const
{
  return (byteOffset != -1);
}

const void* VertexAttribute::getAttribPointer() const
{
  return ((const std::uint8_t*)nullptr) + byteOffset;
}

VertexDescription::VertexDescription()
  : byteCount(0)
  , attrs()
{
}

int VertexDescription::addAttr(const std::string &name, VertexAttributeDataType dataType, int elementCount, bool normalize)
{
  int i = (int)attrs.size();
  attrs.emplace_back(name, dataType, elementCount, normalize, byteCount);
  byteCount += attrs.back().byteCount;
  return i;
}

int VertexDescription::getAttrIndex(const std::string &name) const
{
  for (int i = 0; i < attrs.size(); i++) {
    if (attrs[i].name == name) {
      return i;
    }
  }
  return -1;
}

const VertexAttribute& VertexDescription::getAttr(const std::string &attrName) const
{
  int i = getAttrIndex(attrName);
  return getAttr(i);
}

const VertexAttribute& VertexDescription::getAttr(int i) const
{
  static const VertexAttribute empty = VertexAttribute("", VertexAttributeDataType::UNDEFINED, false, 0, -1);
  return (i >= 0 && i < attrs.size()) ? attrs[i] : empty;
}

RenderData::RenderData()
  : RenderData(VertexData({}, 0), IndexData(PrimitiveType::UNDEFINED))
{
}

RenderData::RenderData(VertexData &&vertData, IndexData &&indexData)
  : _vertData(vertData)
  , _indexData(indexData)
{
}

VertexData::VertexData(const VertexDescription &vertDesc, int vertCount)
  : vertDesc(vertDesc)
  , bytes{}
  , vertCount(vertCount)
  , anchor(0, 0, 0)
{
  bytes.resize(vertCount * vertDesc.byteCount);
}

IndexData::IndexData(PrimitiveType primitiveType)
  : primitiveType(primitiveType)
  , indices16{}
  , indices32{}
{
}

void IndexData::addPoint(int v0)
{
  if (v0 <= std::numeric_limits<std::uint16_t>::max()) {
    indices16.emplace_back(v0);
    return;
  }

  // Convert 16-bit indices to 32-bit
  if (!indices16.empty()) {
    for (std::uint16_t ui16 : indices16) {
      indices32.emplace_back(ui16);
    }
    indices16.clear();
  }

  indices32.emplace_back(v0);
}

void IndexData::addLine(int v0, int v1)
{
  addPoint(v0);
  addPoint(v1);
}

void IndexData::addTriangle(int v0, int v1, int v2)
{
  addPoint(v0);
  addPoint(v1);
  addPoint(v2);
}

void RenderData::uploadToGpu()
{
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, _vertData.bytes.size(), _vertData.bytes.data(), GL_STATIC_DRAW);

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

  if (const ShaderUniform &uniform = shader.getUniform("mtxModelView")) {
    glUniformMatrix4fv(uniform.location, 1, GL_FALSE, modelView.m);
  }

  const Matrix33f normalMatrix = modelView.get3x3(); // Fast, but only works with uniform scaling
  //const Matrix33f normalMatrix = modelView.inverted().transposed().get3x3(); // TODO: Use pre-inverted matrix for better performance
  //const Matrix33f normalMatrix = modelView.get3x3().inverted().transposed();
  if (const ShaderUniform &uniform = shader.getUniform("mtxNormal")) {
    glUniformMatrix3fv(uniform.location, 1, GL_FALSE, normalMatrix.m);
  }

  if (const ShaderUniform &uniform = shader.getUniform("mtxProjection")) {
    glUniformMatrix4fv(uniform.location, 1, GL_FALSE, projection.m);
  }

  if (const ShaderUniform &uniform = shader.getUniform("mtxBones")) {
    if (mtxBones.empty()) {
      glUniformMatrix4fv(uniform.location, 1, GL_FALSE, Matrix44f::identity().m); // Single identity matrix
    }
    else {
      glUniformMatrix4fv(uniform.location, (GLsizei)mtxBones.size(), GL_FALSE, mtxBones.front().m);
    }
  }

  if (const ShaderUniform &uniform = shader.getUniform("useBones")) {
    glUniform1i(uniform.location, !mtxBones.empty());
  }

  if (const ShaderUniform &uniform = shader.getUniform("tex")) {
    glUniform1i(uniform.location, 0);
  }

  if (const ShaderUniform &uniform = shader.getUniform("lightDir")) {
    Vector3f lightDir(20, -40, 100); // TODO
    lightDir = (view * Vector4f(lightDir, 0.0)).xyz.normalized();
    glUniform3fv(uniform.location, 1, lightDir.val);
  }

  if (const ShaderUniform &uniform = shader.getUniform("mtlAmbient")) {
    glUniform3fv(uniform.location, 1, mtl.ambient.val);
  }

  if (const ShaderUniform &uniform = shader.getUniform("mtlDiffuse")) {
    glUniform3fv(uniform.location, 1, mtl.diffuse.val);
  }

  if (const ShaderUniform &uniform = shader.getUniform("mtlSpecular")) {
    glUniform3fv(uniform.location, 1, mtl.specular.val);
  }

  if (const ShaderUniform &uniform = shader.getUniform("mtlEmissive")) {
    glUniform3fv(uniform.location, 1, mtl.emissive.val);
  }

  if (const ShaderUniform &uniform = shader.getUniform("mtlShininess")) {
    glUniform1f(uniform.location, mtl.shininess);
  }

  if (const ShaderUniform &uniform = shader.getUniform("mtlShininessStrength")) {
    glUniform1f(uniform.location, mtl.shininessStrength);
  }

  if (const ShaderUniform &uniform = shader.getUniform("viewportSize")) {
    glUniform2f(uniform.location, (float)viewport.width, (float)viewport.height);
  }

  if (const ShaderUniform &uniform = shader.getUniform("lineWidth")) {
    glUniform1f(uniform.location, mtl.lineWidth);
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

  const VertexDescription &vertDesc = _vertData.vertDesc;
  const int stride = vertDesc.byteCount;

  for (const ShaderAttribute &shaderAttr : shader.attrs) {
    if (const VertexAttribute &attr = vertDesc.getAttr(shaderAttr.name)) {
      glEnableVertexAttribArray(shaderAttr.location);

      const void *attrPtr = attr.getAttribPointer();

      if (attr.dataType == VertexAttributeDataType::FLOAT32) {
        glVertexAttribPointer(shaderAttr.location, attr.elementCount, GL_FLOAT, GL_FALSE, stride, attrPtr);
      }
      else if (attr.dataType == VertexAttributeDataType::FLOAT64) {
        glVertexAttribLPointer(shaderAttr.location, attr.elementCount, GL_DOUBLE, stride, attrPtr);
      }
      else if (attr.dataType == VertexAttributeDataType::INT8) {
        if (attr.normalize) {
          glVertexAttribPointer(shaderAttr.location, attr.elementCount, GL_BYTE, GL_TRUE, stride, attrPtr);
        }
        else { // TODO: Do we ever want to call glVertexAttribPointer() instead of glVertexAttribIPointer() for integer data types when attr.normalize is false?
          glVertexAttribIPointer(shaderAttr.location, attr.elementCount, GL_BYTE, stride, attrPtr);
        }
      }
      else if (attr.dataType == VertexAttributeDataType::INT16) {
        if (attr.normalize) {
          glVertexAttribPointer(shaderAttr.location, attr.elementCount, GL_SHORT, GL_TRUE, stride, attrPtr);
        }
        else {
          glVertexAttribIPointer(shaderAttr.location, attr.elementCount, GL_SHORT, stride, attrPtr);
        }
      }
      else if (attr.dataType == VertexAttributeDataType::INT32) {
        if (attr.normalize) {
          glVertexAttribPointer(shaderAttr.location, attr.elementCount, GL_INT, GL_TRUE, stride, attrPtr);
        }
        else {
          glVertexAttribIPointer(shaderAttr.location, attr.elementCount, GL_INT, stride, attrPtr);
        }
      }
      else if (attr.dataType == VertexAttributeDataType::UINT8) {
        if (attr.normalize) {
          glVertexAttribPointer(shaderAttr.location, attr.elementCount, GL_UNSIGNED_BYTE, GL_TRUE, stride, attrPtr);
        }
        else {
          glVertexAttribIPointer(shaderAttr.location, attr.elementCount, GL_UNSIGNED_BYTE, stride, attrPtr);
        }
      }
      else if (attr.dataType == VertexAttributeDataType::UINT16) {
        if (attr.normalize) {
          glVertexAttribPointer(shaderAttr.location, attr.elementCount, GL_UNSIGNED_SHORT, GL_TRUE, stride, attrPtr);
        }
        else {
          glVertexAttribIPointer(shaderAttr.location, attr.elementCount, GL_UNSIGNED_SHORT, stride, attrPtr);
        }
      }
      else if (attr.dataType == VertexAttributeDataType::UINT32) {
        if (attr.normalize) {
          glVertexAttribPointer(shaderAttr.location, attr.elementCount, GL_UNSIGNED_INT, GL_TRUE, stride, attrPtr);
        }
        else {
          glVertexAttribIPointer(shaderAttr.location, attr.elementCount, GL_UNSIGNED_INT, stride, attrPtr);
        }
      }
      else {
        Log::warning("Unsupported VertexAttributeDataType: %d", attr.dataType);
      }
    }
    else {
      glDisableVertexAttribArray(shaderAttr.location);
    }
  }

  GLenum mode = GL_POINTS; // Default fallback
  switch (_indexData.primitiveType) {
  case PrimitiveType::POINTS: mode = GL_POINTS; break;
  case PrimitiveType::LINES: mode = GL_LINES; break;
  case PrimitiveType::TRIANGLES: mode = GL_TRIANGLES; break;
  default: Log::warning("Unsupported PrimitiveType: %d", mode); break;
  }

  if (!_indexData.indices16.empty()) {
    glDrawElements(mode, (int)_indexData.indices16.size(), GL_UNSIGNED_SHORT, _indexData.indices16.data());
  }
  else if (!_indexData.indices32.empty()) {
    glDrawElements(mode, (int)_indexData.indices32.size(), GL_UNSIGNED_INT, _indexData.indices32.data());
  }
  else {
    glDrawArrays(mode, 0, _vertData.vertCount);
  }
}

}
