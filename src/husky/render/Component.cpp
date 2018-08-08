#include <husky/render/Component.hpp>
#include <husky/render/Entity.hpp>
#include <husky/Log.hpp>

namespace husky {

IComponent::IComponent()
  : owner(nullptr)
{
}

IComponent::~IComponent()
{
}

DebugDrawComponent::DebugDrawComponent()
{
}

DebugDrawComponent::~DebugDrawComponent()
{
}

static bool initialized = false;
static RenderData boxRenderData;
static Material boxMaterial;
static RenderData sphereRenderData;
static Material sphereMaterial;
static RenderData axesRenderData;
static Material axesMaterial;

void DebugDrawComponent::init(Entity *owner)
{
  assert(this->owner == nullptr);
  assert(owner != nullptr);
  this->owner = owner;

  if (!initialized) {
    initialized = true;

    {
      Mesh boxMesh = Mesh::box(1, 1, 1);
      boxMesh.convertFacesToWireframeLines();
      boxRenderData = boxMesh.getRenderData();

      boxMaterial = Material({ 1, 1, 1 });
      boxMaterial.wireframe = true;
      boxMaterial.lineWidth = 2.0f;
    }

    {
      Mesh sphereMesh = Mesh::sphere(1);
      sphereMesh.convertFacesToWireframeLines();
      sphereRenderData = sphereMesh.getRenderData();

      sphereMaterial = Material({ 0, 1, 0 });
      sphereMaterial.wireframe = true;
      sphereMaterial.lineWidth = 1.0f;
    }

    {
      axesRenderData = Mesh::axes().getRenderData();

      axesMaterial = Material({ 1, 1, 1 });
      axesMaterial.specular = { 0, 0, 0 };
    }
  }
}

void DebugDrawComponent::draw(const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection) const
{
  static const Shader defaultShader = Shader::getDefaultShader(false, false);
  static const Shader lineShader = Shader::getDefaultLineShader();

  axesRenderData.draw(defaultShader, axesMaterial, viewport, view, modelView, projection);

  Matrix44f boxModelView(modelView * Matrix44f::translate(Vector3f(owner->bboxLocal.center())) * Matrix44f::scale(Vector3f(owner->bboxLocal.size())));
  boxRenderData.draw(lineShader, boxMaterial, viewport, view, boxModelView, projection);
  
  Matrix44f sphereModelView(modelView * Matrix44f::translate(Vector3f(owner->bsphereLocal.center)) * Matrix44f::scale(Vector3f((float)owner->bsphereLocal.radius)));
  sphereRenderData.draw(lineShader, sphereMaterial, viewport, view, sphereModelView, projection);

  for (const ModelNode *node : owner->modelInstance.model->getNodesFlatList()) {
    // TODO
  }
}

}
