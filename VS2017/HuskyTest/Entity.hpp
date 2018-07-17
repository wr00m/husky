#pragma once

#include <husky/Log.hpp>
#include <husky/mesh/Primitive.hpp>
#include <husky/mesh/Model.hpp>
#include <husky/render/Viewport.hpp>
#include <husky/render/Camera.hpp>
#include "GLMaterial.hpp"

class Entity
{
private:
  static void draw(const GLMaterial &mtl, const husky::RenderData &renderData, const husky::Viewport &viewport, const husky::Matrix44f &modelView, const husky::Matrix44f &projection, GLuint vbo, GLuint vao)
  {
    if (mtl.shaderProgram == 0) {
      husky::Log::warning("Invalid shader program");
      return;
    }

    glUseProgram(mtl.shaderProgram);

    if (mtl.modelViewLocation != -1) {
      glUniformMatrix4fv(mtl.modelViewLocation, 1, GL_FALSE, modelView.m);
    }

    if (mtl.projectionLocation != -1) {
      glUniformMatrix4fv(mtl.projectionLocation, 1, GL_FALSE, projection.m);
    }

    if (mtl.texLocation != -1) {
      glUniform1i(mtl.texLocation, 0);
    }

    if (mtl.viewportSizeLocation != -1) {
      glUniform2f(mtl.viewportSizeLocation, (float)viewport.width, (float)viewport.height);
    }

    if (mtl.lineWidthLocation != -1) {
      glUniform1f(mtl.lineWidthLocation, mtl.lineWidth);
    }

    //glLineWidth(2.f);

    if (mtl.textureHandle != 0) {
      glActiveTexture(GL_TEXTURE0 + 0);
      glBindTexture(GL_TEXTURE_2D, mtl.textureHandle);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);

    if (mtl.vertPositionLocation != -1 && renderData.hasAttrib(husky::RenderData::Attribute::POSITION)) {
      glEnableVertexAttribArray(mtl.vertPositionLocation);
      glVertexAttribPointer(mtl.vertPositionLocation, 3, GL_FLOAT, GL_FALSE, renderData.vertByteCount, renderData.attribPointer(husky::RenderData::Attribute::POSITION));
    }

    if (mtl.vertNormalLocation != -1 && renderData.hasAttrib(husky::RenderData::Attribute::NORMAL)) {
      glEnableVertexAttribArray(mtl.vertNormalLocation);
      glVertexAttribPointer(mtl.vertNormalLocation, 3, GL_FLOAT, GL_FALSE, renderData.vertByteCount, renderData.attribPointer(husky::RenderData::Attribute::NORMAL));
    }

    if (mtl.vertTexCoordLocation != -1 && renderData.hasAttrib(husky::RenderData::Attribute::TEXCOORD)) {
      glEnableVertexAttribArray(mtl.vertTexCoordLocation);
      glVertexAttribPointer(mtl.vertTexCoordLocation, 2, GL_FLOAT, GL_FALSE, renderData.vertByteCount, renderData.attribPointer(husky::RenderData::Attribute::TEXCOORD));
    }

    if (mtl.vertColorLocation != -1 && renderData.hasAttrib(husky::RenderData::Attribute::COLOR)) {
      glEnableVertexAttribArray(mtl.vertColorLocation);
      glVertexAttribPointer(mtl.vertColorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, renderData.vertByteCount, renderData.attribPointer(husky::RenderData::Attribute::COLOR));
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
  Entity(const std::string &name, const GLMaterial &mtl, const GLMaterial &lineMtl, const husky::Model &mdl)
    : name(name)
    , mtl(mtl)
    , lineMtl(lineMtl)
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
    draw(mtl, renderData, viewport, modelView, projection, vbo, vao);

    if (drawBbox) {
      glDisable(GL_CULL_FACE);
      draw(lineMtl, bboxRenderData, viewport, modelView, projection, vboBbox, vaoBbox);
    }
  }

  std::string name;
  husky::Matrix44d transform = husky::Matrix44d::identity();
  GLMaterial mtl;
  GLMaterial lineMtl;
  husky::RenderData renderData;
  husky::BoundingBox bboxLocal;
  husky::RenderData bboxRenderData;

private:
  GLuint vbo;
  GLuint vao;
  GLuint vboBbox;
  GLuint vaoBbox;
};
