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
static RenderData boneRenderData;
static Material boneMaterial;
static Material boneMaterialAnimated;

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

    {
      boneRenderData = Mesh::cone(0.01, 0.1, true, 6).getRenderData();

      boneMaterial = Material({ 1, 1, 1 });
      boneMaterial.depthTest = false;

      boneMaterialAnimated = Material({ 1, 0, 0 });
      boneMaterialAnimated.depthTest = false;
    }
  }
}

void DebugDrawComponent::draw(const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection) const
{
  static const Shader defaultShader = Shader::getDefaultShader(false, false);
  static const Shader lineShader = Shader::getLineShader();

  // Draw Entity axes
  axesRenderData.draw(defaultShader, axesMaterial, viewport, view, modelView, projection);

  {
    // Draw ModelInstance bounds, etc.
    const ModelInstance &modelInstance = owner->modelInstance;
    const Model *model = modelInstance.model;
    const Matrix44f instanceModelView(modelView * (Matrix44f)modelInstance.mtxTransform);

    Matrix44f boxModelView(instanceModelView * Matrix44f::translate(Vector3f(model->bboxLocal.center())) * Matrix44f::scale(Vector3f(model->bboxLocal.size())));
    boxRenderData.draw(lineShader, boxMaterial, viewport, view, boxModelView, projection);

    //Matrix44f sphereModelView(instanceModelView * Matrix44f::translate(Vector3f(model->bsphereLocal.center)) * Matrix44f::scale(Vector3f((float)model->bsphereLocal.radius)));
    //sphereRenderData.draw(lineShader, sphereMaterial, viewport, view, sphereModelView, projection);

    for (const auto &pair : modelInstance.animNodes) {
      const AnimatedNode &animNode = pair.second;
      const Material &mtl = (animNode.animated ? boneMaterialAnimated : boneMaterial);
      Matrix44f boneModelView(instanceModelView * (Matrix44f)animNode.mtxRelToModel);
      boneRenderData.draw(defaultShader, mtl, viewport, view, boneModelView, projection);
    }
  }
}

}
