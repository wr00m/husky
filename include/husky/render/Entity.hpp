#pragma once

#include <husky/mesh/Model.hpp>
#include <memory>

namespace husky {

class IComponent;

class HUSKY_DLL Entity
{
public:
  Entity(const std::string &name, const Shader *shader, const Model *model);

  void update(double timeDelta);
  void draw(const Viewport &viewport, const Camera &cam) const;
  void calcBbox();
  const Matrix44d& getTransform() const;
  void setTransform(const Matrix44d &mtxTransform);
  void addComponent(std::unique_ptr<IComponent> &&component);
  void removeComponent(IComponent *component);

  template<class T>
  void addComponent() { addComponent(std::make_unique<husky::DebugDrawComponent>()); }

  template<class T>
  void removeComponent() { removeComponent(getComponent<T>()); }

  template<class T>
  T* getComponent()
  {
    for (IComponent *component : components) {
      if (T *t = dynamic_cast<T*>(component)) {
        return t;
      }
    }
    return nullptr;
  }

  std::string name;
  const Shader *shader;
  ModelInstance modelInstance;
  Box bboxLocal;
  Sphere bsphereLocal;

private:
  Matrix44d mtxTransform;
  std::vector<IComponent*> components;
};

}
