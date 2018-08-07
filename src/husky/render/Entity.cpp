#include <husky/render/Entity.hpp>
#include <husky/render/Component.hpp>
#include <husky/mesh/Mesh.hpp>
#include <husky/Log.hpp>

namespace husky {

Entity::Entity(const std::string &name, const Shader *shader, const Model *model)
  : name(name)
  , mtxTransform(Matrix44d::identity())
  , shader(shader)
  , modelInstance(model)
  , bboxLocal()
  , bsphereLocal()
  , components()
{
  calcBbox();
}

void Entity::draw(const Viewport &viewport, const Camera &cam) const
{
  const Matrix44f view(cam.view);
  const Matrix44f modelView(cam.view * mtxTransform);
  const Matrix44f projection(cam.proj);

  modelInstance.draw(*shader, viewport, view, modelView, projection);

  for (const IComponent *component : components) {
    component->draw(viewport, view, modelView, projection);
  }
}

void Entity::calcBbox()
{
  // TODO: Combine boxes of multiple ModelInstances
  bboxLocal = modelInstance.model->bboxLocal;
  bsphereLocal = modelInstance.model->bsphereLocal;
}

const Matrix44d& Entity::getTransform() const
{
  return mtxTransform;
}

void Entity::setTransform(const Matrix44d &mtxTransform)
{
  this->mtxTransform = mtxTransform;
}

void Entity::addComponent(std::unique_ptr<IComponent> &&component)
{
  if (component != nullptr) {
    components.emplace_back(component.release());
    components.back()->init(this);
  }
}

void Entity::removeComponent(IComponent *component)
{
  if (component == nullptr) {
    return;
  }

  const auto it = std::find(components.begin(), components.end(), component);
  if (it != components.end()) {
    delete *it;
    components.erase(it);
  }
}

}
