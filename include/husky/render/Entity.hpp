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
  Entity(const std::string &name, const Shader *shader, const Shader *lineShader, Model *model);

  void draw(const Viewport &viewport, const Camera &cam, bool drawBbox) const;

  std::string name;
  Matrix44d transform = Matrix44d::identity();
  const Shader *shader;
  const Shader *lineShader;
  ModelInstance modelInstance;
  BoundingBox bboxLocal;
  Model bboxModel;
};

}
