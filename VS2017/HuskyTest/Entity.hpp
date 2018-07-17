#pragma once

#include <husky/Log.hpp>
#include <husky/mesh/Primitive.hpp>
#include <husky/mesh/Model.hpp>
#include <husky/render/Viewport.hpp>
#include <husky/render/Camera.hpp>
#include "Shader.hpp"

class Entity
{
private:
  static void draw(const Shader &shader, const husky::RenderData &renderData, const husky::Viewport &viewport, const husky::Matrix44f &modelView, const husky::Matrix44f &projection, GLuint vbo, GLuint vao)
  {
    if (shader.shaderProgram == 0) {
      husky::Log::warning("Invalid shader program");
      return;
    }

    glUseProgram(shader.shaderProgram);

    if (shader.modelViewLocation != -1) {
      glUniformMatrix4fv(shader.modelViewLocation, 1, GL_FALSE, modelView.m);
    }

    if (shader.projectionLocation != -1) {
      glUniformMatrix4fv(shader.projectionLocation, 1, GL_FALSE, projection.m);
    }

    if (shader.texLocation != -1) {
      glUniform1i(shader.texLocation, 0);
    }

    if (shader.viewportSizeLocation != -1) {
      glUniform2f(shader.viewportSizeLocation, (float)viewport.width, (float)viewport.height);
    }

    if (shader.lineWidthLocation != -1) {
      glUniform1f(shader.lineWidthLocation, shader.lineWidth);
    }

    //glLineWidth(2.f);

    if (shader.textureHandle != 0) {
      glActiveTexture(GL_TEXTURE0 + 0);
      glBindTexture(GL_TEXTURE_2D, shader.textureHandle);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);

    if (shader.vertPositionLocation != -1 && renderData.hasAttrib(husky::RenderData::Attribute::POSITION)) {
      glEnableVertexAttribArray(shader.vertPositionLocation);
      glVertexAttribPointer(shader.vertPositionLocation, 3, GL_FLOAT, GL_FALSE, renderData.vertByteCount, renderData.attribPointer(husky::RenderData::Attribute::POSITION));
    }

    if (shader.vertNormalLocation != -1 && renderData.hasAttrib(husky::RenderData::Attribute::NORMAL)) {
      glEnableVertexAttribArray(shader.vertNormalLocation);
      glVertexAttribPointer(shader.vertNormalLocation, 3, GL_FLOAT, GL_FALSE, renderData.vertByteCount, renderData.attribPointer(husky::RenderData::Attribute::NORMAL));
    }

    if (shader.vertTexCoordLocation != -1 && renderData.hasAttrib(husky::RenderData::Attribute::TEXCOORD)) {
      glEnableVertexAttribArray(shader.vertTexCoordLocation);
      glVertexAttribPointer(shader.vertTexCoordLocation, 2, GL_FLOAT, GL_FALSE, renderData.vertByteCount, renderData.attribPointer(husky::RenderData::Attribute::TEXCOORD));
    }

    if (shader.vertColorLocation != -1 && renderData.hasAttrib(husky::RenderData::Attribute::COLOR)) {
      glEnableVertexAttribArray(shader.vertColorLocation);
      glVertexAttribPointer(shader.vertColorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, renderData.vertByteCount, renderData.attribPointer(husky::RenderData::Attribute::COLOR));
    }

    GLenum mode = GL_POINTS; // Default fallback;
    switch (renderData.mode) {
    case     husky::RenderData::Mode::POINTS:    mode = GL_POINTS;    break;
    case     husky::RenderData::Mode::LINES:     mode = GL_LINES;     break;
    case     husky::RenderData::Mode::TRIANGLES: mode = GL_TRIANGLES; break;
    default: husky::Log::warning("Unsupported RenderData::Mode");     break;
    }

    glDrawElements(mode, (int)renderData.indices.size(), GL_UNSIGNED_SHORT, renderData.indices.data());
  }

public:
  Entity(const std::string &name, const Shader &shader, const Shader &lineShader, const husky::Model &mdl)
    : name(name)
    , shader(shader)
    , lineShader(lineShader)
  {
    // TODO: Preserve meshes and use materials!
    husky::SimpleMesh combinedMesh;
    for (const husky::SimpleMesh &mesh : mdl.transformedMeshes) {
      combinedMesh.addMesh(mesh);
    }

    renderData = combinedMesh.getRenderData();

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, renderData.bytes.size(), renderData.bytes.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);

    bboxLocal = combinedMesh.getBoundingBox();
    husky::Vector3d bboxSize = bboxLocal.size();
    husky::SimpleMesh bboxMesh = husky::Primitive::box(bboxSize.x, bboxSize.y, bboxSize.z);
    bboxMesh.setAllVertexColors({ 255, 255, 255, 255 });
    bboxMesh.transform(husky::Matrix44d::translate(bboxLocal.center()));
    bboxRenderData = bboxMesh.getRenderDataWireframe();

    glGenBuffers(1, &vboBbox);
    glBindBuffer(GL_ARRAY_BUFFER, vboBbox);
    glBufferData(GL_ARRAY_BUFFER, bboxRenderData.bytes.size(), bboxRenderData.bytes.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vaoBbox);
    //glBindVertexArray(vaoBbox);
  }

  void draw(const husky::Viewport &viewport, const husky::Camera &cam, bool drawBbox) const
  {
    const husky::Matrix44f modelView(cam.view * transform);
    const husky::Matrix44f projection(cam.projection);

    glEnable(GL_CULL_FACE);
    draw(shader, renderData, viewport, modelView, projection, vbo, vao);

    if (drawBbox) {
      glDisable(GL_CULL_FACE);
      draw(lineShader, bboxRenderData, viewport, modelView, projection, vboBbox, vaoBbox);
    }
  }

  std::string name;
  husky::Matrix44d transform = husky::Matrix44d::identity();
  Shader shader;
  Shader lineShader;
  husky::RenderData renderData;
  husky::BoundingBox bboxLocal;
  husky::RenderData bboxRenderData;

private:
  GLuint vbo;
  GLuint vao;
  GLuint vboBbox;
  GLuint vaoBbox;
};
