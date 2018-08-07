#pragma once

#include <husky/mesh/Model.hpp>

namespace husky {

class HUSKY_DLL Entity
{
public:
  Entity(const std::string &name, const Shader *shader, Model *model);

  void draw(const Viewport &viewport, const Camera &cam, bool drawBbox) const;
  void calcBbox();
  const Matrix44d& getTransform() const;
  void setTransform(const Matrix44d &mtxTransform);

  std::string name;
  const Shader *shader;
  ModelInstance modelInstance;
  Box bboxLocal;

private:
  Matrix44d mtxTransform;
  Model bboxLocalModel;
};

}
