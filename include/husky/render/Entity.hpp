#pragma once

#include <husky/mesh/BoundingBox.hpp>
#include <husky/mesh/Model.hpp>
#include <husky/render/Viewport.hpp>
#include <husky/render/Camera.hpp>
#include <husky/render/Shader.hpp>

namespace husky {

class HUSKY_DLL Entity
{
public:
  Entity(const std::string &name, const Shader *shader, const Shader *lineShader, husky::Model *model);

  void draw(const husky::Viewport &viewport, const husky::Camera &cam, bool drawBbox) const;

  std::string name;
  husky::Matrix44d transform = husky::Matrix44d::identity();
  const Shader *shader;
  const Shader *lineShader;
  husky::ModelInstance modelInstance;
  husky::BoundingBox bboxLocal;
  husky::Model bboxModel;
};

}
