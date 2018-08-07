#include <husky/render/Entity.hpp>
#include <husky/mesh/Mesh.hpp>
#include <husky/Log.hpp>

namespace husky {

Entity::Entity(const std::string &name, const Shader *shader, Model *model)
  : name(name)
  , mtxTransform(Matrix44d::identity())
  , shader(shader)
  , modelInstance(model)
  , bboxLocal()
  , bboxLocalModel()
{
  calcBbox();
}

void Entity::draw(const Viewport &viewport, const Camera &cam, bool drawBbox) const
{
  const Matrix44f view(cam.view);
  const Matrix44f modelView(cam.view * mtxTransform);
  const Matrix44f projection(cam.proj);

  modelInstance.draw(*shader, viewport, view, modelView, projection);

  if (drawBbox) {
    static const Shader lineShader = Shader::getDefaultLineShader();
    bboxLocalModel.draw(lineShader, viewport, view, modelView, projection);
    bsphereLocalModel.draw(lineShader, viewport, view, modelView, projection);
  }
}

void Entity::calcBbox()
{
  // TODO: Combine boxes of multiple ModelInstances
  bboxLocal = modelInstance.model->bboxLocal;
  bsphereLocal = modelInstance.model->bsphereLocal;

  //for (const auto &mm : modelInstance.model->bboxLocal) {
  //  bbox.expand((mtxTransform * Vector4d(mm.bboxLocal.min, 1.0)).xyz);
  //  bbox.expand((mtxTransform * Vector4d(mm.bboxLocal.max, 1.0)).xyz);
  //}

  Vector3d bboxSize = bboxLocal.size();
  Mesh bboxMesh = Mesh::box(bboxSize.x, bboxSize.y, bboxSize.z);
  bboxMesh.convertFacesToWireframeLines();
  bboxMesh.translate(bboxLocal.center());

  Mesh bsphereMesh = Mesh::sphere(bsphereLocal.radius);
  bsphereMesh.convertFacesToWireframeLines();
  bsphereMesh.translate(bsphereLocal.center);

  Material mtlBox({ 1, 1, 1 });
  mtlBox.wireframe = true;
  mtlBox.lineWidth = 2.0f;
  bboxLocalModel = Model(std::move(bboxMesh), mtlBox);

  Material mtlSphere({ 0, 1, 0 });
  mtlSphere.wireframe = true;
  mtlSphere.lineWidth = 1.0f;
  bsphereLocalModel = Model(std::move(bsphereMesh), mtlSphere);
}

const Matrix44d& Entity::getTransform() const
{
  return mtxTransform;
}

void Entity::setTransform(const Matrix44d &mtxTransform)
{
  this->mtxTransform = mtxTransform;
}

}
