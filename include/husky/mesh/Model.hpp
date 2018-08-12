#pragma once

#include <husky/mesh/Animation.hpp>
#include <husky/math/Box.hpp>
#include <husky/mesh/Material.hpp>
#include <husky/mesh/Mesh.hpp>
#include <husky/render/Camera.hpp>
#include <husky/render/Shader.hpp>
#include <husky/render/Viewport.hpp>
#include <memory>
#include <string>

namespace husky {

class Shader;
class Viewport;

class HUSKY_DLL ModelNode // Coordinate frame
{
public:
  ModelNode(const std::string &name, const Matrix44d &mtxRelToParent, const Matrix44d *mtxParentRelToModel);
  ~ModelNode();

  std::string name;
  //const ModelNode *parent; // std::weak_ptr?
  std::vector<ModelNode*> children; // std::unique_ptr?
  Matrix44d mtxRelToParent;
  Matrix44d mtxRelToModel;
  std::vector<int> meshIndices;
};

class HUSKY_DLL ModelMesh
{
public:
  ModelMesh(const std::string &name, int materialIndex, Mesh &&mesh);

  std::string name;
  Mesh mesh;
  int materialIndex;
  Box bboxLocal;
  Sphere bsphereLocal;
  RenderData renderData;
};

class HUSKY_DLL Model
{
public:
  static Model load(const std::string &filePath);

  Model(const std::string &name);
  Model(Mesh &&mesh, const Material &mtl);

  int addMaterial(const Material &mtl);
  int addMesh(ModelMesh &&mm);
  const Material& getMaterial(int mtlIndex) const;
  void draw(const Shader &shader, const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection, const std::map<std::string, AnimatedNode> &animNodes) const;
  void calcBbox();

  std::string name;
  std::vector<Material> materials;
  std::vector<ModelMesh> meshes;
  std::vector<Animation> animations;
  ModelNode *root; // std::unique_ptr?
  Box bboxLocal; // Does not take animation into consideration
  Sphere bsphereLocal; // Does not take animation into consideration
  std::vector<const ModelNode*> getNodesFlatList() const;

private:
  void getNodesRecursive(const ModelNode *node, std::vector<const ModelNode*> &nodes) const;
};

class HUSKY_DLL ModelInstance
{
public:
  ModelInstance(const Model *model);

  void animate(double timeDelta);
  void draw(const Shader &shader, const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection) const;
  void setAnimationIndex(int i);
  const Animation* getActiveAnimation() const;

  const Model *model; // std::shared_ptr?
  int animationIndex;
  double animationTime;
  std::map<std::string, AnimatedNode> animNodes;
};

}
