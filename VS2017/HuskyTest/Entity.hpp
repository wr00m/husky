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
  static void draw(const Shader &shader, const husky::Model &model, const husky::Viewport &viewport, const husky::Matrix44f &modelView, const husky::Matrix44f &projection)
  {
    static const husky::Material fallbackMtl;

    for (int iMesh = 0; iMesh < model.meshRenderDatas.size(); iMesh++) {
      const husky::Material *mtl = nullptr;
      if (iMesh < model.meshMaterialIndices.size() && model.meshMaterialIndices[iMesh] >= 0 && model.meshMaterialIndices[iMesh] < model.materials.size()) {
        mtl = &model.materials[model.meshMaterialIndices[iMesh]];
      }
      else {
        husky::Log::warning("No material");
        mtl = &fallbackMtl;
      }

      const husky::RenderData &renderData = model.meshRenderDatas[iMesh];
      draw(shader, *mtl, renderData, viewport, modelView, projection);
    }
  }

  static void draw(const Shader &shader, const husky::Material &mtl, const husky::RenderData &renderData, const husky::Viewport &viewport, const husky::Matrix44f &modelView, const husky::Matrix44f &projection)
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

    if (shader.diffuseLightColorLocation != -1) {
      glUniform3fv(shader.diffuseLightColorLocation, 1, mtl.diffuseColor.val);
    }

    if (shader.ambientLightColorLocation != -1) {
      glUniform3fv(shader.ambientLightColorLocation, 1, mtl.ambientColor.val);
    }

    if (shader.viewportSizeLocation != -1) {
      glUniform2f(shader.viewportSizeLocation, (float)viewport.width, (float)viewport.height);
    }

    if (shader.lineWidthLocation != -1) {
      glUniform1f(shader.lineWidthLocation, mtl.lineWidth);
    }

    if (shader.textureHandle != 0) {
      glActiveTexture(GL_TEXTURE0 + 0);
      glBindTexture(GL_TEXTURE_2D, shader.textureHandle);
    }

    if (renderData.vbo == 0) {
      husky::Log::warning("renderData.vbo is 0");
      return;
    }

    if (renderData.vao == 0) {
      husky::Log::warning("renderData.vao is 0");
      return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, renderData.vbo);
    glBindVertexArray(renderData.vao);

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

    GLenum mode = GL_POINTS; // Default fallback
    switch (renderData.mode) {
    case     husky::RenderData::Mode::POINTS:    mode = GL_POINTS;          break;
    case     husky::RenderData::Mode::LINES:     mode = GL_LINES;           break;
    case     husky::RenderData::Mode::TRIANGLES: mode = GL_TRIANGLES;       break;
    default: husky::Log::warning("Unsupported RenderData::Mode: %d", mode); break;
    }

    glDrawElements(mode, (int)renderData.indices.size(), GL_UNSIGNED_SHORT, renderData.indices.data());
  }

public:
  Entity(const std::string &name, const Shader &shader, const Shader &lineShader, const husky::Model &&mdl)
    : name(name)
    , shader(shader)
    , lineShader(lineShader)
    , model(mdl)
  {
    for (const auto &renderData : mdl.meshRenderDatas) {
      for (int iVert = 0; iVert < renderData.vertCount; iVert++) {
        husky::Vector3f pos = renderData.getValue<husky::Vector3f>(iVert, husky::RenderData::Attribute::POSITION);
        pos += renderData.anchor;
        bboxLocal.expand(pos);
      }
    }
    
    husky::Vector3d bboxSize = bboxLocal.size();
    husky::SimpleMesh bboxMesh = husky::Primitive::box(bboxSize.x, bboxSize.y, bboxSize.z);
    bboxMesh.transform(husky::Matrix44d::translate(bboxLocal.center()));
    bboxModel = { bboxMesh.getRenderDataWireframe(), husky::Material({ 1, 1, 1 }) };
  }

  void draw(const husky::Viewport &viewport, const husky::Camera &cam, bool drawBbox) const
  {
    const husky::Matrix44f modelView(cam.view * transform);
    const husky::Matrix44f projection(cam.projection);

    glEnable(GL_CULL_FACE); // TODO: Move to Material
    draw(shader, model, viewport, modelView, projection);

    if (drawBbox) {
      glDisable(GL_CULL_FACE); // TODO: Move to Material
      draw(lineShader, bboxModel, viewport, modelView, projection);
    }
  }

  std::string name;
  husky::Matrix44d transform = husky::Matrix44d::identity();
  Shader shader;
  Shader lineShader;
  husky::Model model;
  husky::BoundingBox bboxLocal;
  husky::Model bboxModel;
};
