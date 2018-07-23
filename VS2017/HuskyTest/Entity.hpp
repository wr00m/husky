#pragma once

#include <husky/Log.hpp>
#include <husky/mesh/SimpleMesh.hpp>
#include <husky/mesh/BoundingBox.hpp>
#include <husky/mesh/Model.hpp>
#include <husky/render/Viewport.hpp>
#include <husky/render/Camera.hpp>
#include "Shader.hpp"

class Entity
{
private:
  static void draw(
    const Shader &shader,
    const husky::Model &model,
    const husky::Viewport &viewport,
    const husky::Matrix44f &view,
    const husky::Matrix44f &modelView,
    const husky::Matrix44f &projection)
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
      draw(shader, *mtl, renderData, viewport, view, modelView, projection);
    }
  }

  static void draw(
    const Shader &shader,
    const husky::Material &mtl,
    const husky::RenderData &renderData,
    const husky::Viewport &viewport,
    const husky::Matrix44f &view,
    const husky::Matrix44f &modelView,
    const husky::Matrix44f &projection)
  {
    if (shader.shaderProgram == 0) {
      husky::Log::warning("Invalid shader program");
      return;
    }

    if (mtl.twoSided) {
      glDisable(GL_CULL_FACE);
    }
    else {
      glEnable(GL_CULL_FACE);
    }

    glUseProgram(shader.shaderProgram);

    int varLocation;

    if (shader.getUniformLocation("mtxModelView", varLocation)) {
      glUniformMatrix4fv(varLocation, 1, GL_FALSE, modelView.m);
    }

    const husky::Matrix33f normalMatrix = modelView.get3x3();
    //const husky::Matrix33f normalMatrix = modelView.inverted().transposed().get3x3(); // TODO: Use pre-inverted matrix for better performance
    //const husky::Matrix33f normalMatrix = modelView.get3x3().inverted().transposed();
    if (shader.getUniformLocation("mtxNormal", varLocation)) {
      glUniformMatrix3fv(varLocation, 1, GL_FALSE, normalMatrix.m);
    }

    if (shader.getUniformLocation("mtxProjection", varLocation)) {
      glUniformMatrix4fv(varLocation, 1, GL_FALSE, projection.m);
    }

    if (shader.getUniformLocation("tex", varLocation)) {
      glUniform1i(varLocation, 0);
    }

    if (shader.getUniformLocation("lightDir", varLocation)) {
      husky::Vector3f lightDir(20, -40, 100); // TODO
      lightDir = (view * husky::Vector4f(lightDir, 0.0)).xyz.normalized();
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

    const void *attrPtr = nullptr;

    if (shader.getAttributeLocation("vertPosition", varLocation) && renderData.getAttribPointer(husky::RenderData::Attribute::POSITION, attrPtr)) {
      glEnableVertexAttribArray(varLocation);
      glVertexAttribPointer(varLocation, 3, GL_FLOAT, GL_FALSE, renderData.vertByteCount, attrPtr);
    }

    if (shader.getAttributeLocation("vertNormal", varLocation) && renderData.getAttribPointer(husky::RenderData::Attribute::NORMAL, attrPtr)) {
      glEnableVertexAttribArray(varLocation);
      glVertexAttribPointer(varLocation, 3, GL_FLOAT, GL_FALSE, renderData.vertByteCount, attrPtr);
    }

    if (shader.getAttributeLocation("vertTexCoord", varLocation) && renderData.getAttribPointer(husky::RenderData::Attribute::TEXCOORD, attrPtr)) {
      glEnableVertexAttribArray(varLocation);
      glVertexAttribPointer(varLocation, 2, GL_FLOAT, GL_FALSE, renderData.vertByteCount, attrPtr);
    }

    if (shader.getAttributeLocation("vertColor", varLocation) && renderData.getAttribPointer(husky::RenderData::Attribute::COLOR, attrPtr)) {
      glEnableVertexAttribArray(varLocation);
      glVertexAttribPointer(varLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, renderData.vertByteCount, attrPtr);
    }

    if (shader.getAttributeLocation("vertBoneIndices", varLocation) && renderData.getAttribPointer(husky::RenderData::Attribute::BONE_INDICES, attrPtr)) {
      glEnableVertexAttribArray(varLocation);
      glVertexAttribPointer(varLocation, 4, GL_UNSIGNED_BYTE, GL_FALSE, renderData.vertByteCount, attrPtr);
    }

    if (shader.getAttributeLocation("vertBoneWeights", varLocation) && renderData.getAttribPointer(husky::RenderData::Attribute::BONE_WEIGHTS, attrPtr)) {
      glEnableVertexAttribArray(varLocation);
      glVertexAttribPointer(varLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, renderData.vertByteCount, attrPtr);
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
    husky::SimpleMesh bboxMesh = husky::SimpleMesh::box(bboxSize.x, bboxSize.y, bboxSize.z);
    bboxMesh.transform(husky::Matrix44d::translate(bboxLocal.center()));
    bboxModel = { bboxMesh.getRenderDataWireframe(), husky::Material({ 1, 1, 1 }) };
  }

  void draw(const husky::Viewport &viewport, const husky::Camera &cam, bool drawBbox) const
  {
    const husky::Matrix44f view(cam.view);
    const husky::Matrix44f modelView(cam.view * transform);
    const husky::Matrix44f projection(cam.projection);

    draw(shader, model, viewport, view, modelView, projection);

    if (drawBbox) {
      draw(lineShader, bboxModel, viewport, view, modelView, projection);
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
