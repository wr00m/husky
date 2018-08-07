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
  ModelNode(const std::string &name, const Matrix44d &mtxRelToParent, const ModelNode *parent);
  ~ModelNode();

  std::string name;
  const ModelNode *parent; // std::weak_ptr?
  std::vector<const ModelNode*> children; // std::unique_ptr?
  Matrix44d mtxRelToParent;
  Matrix44d mtxRelToModel;
  std::vector<int> meshIndices;
};

class HUSKY_DLL ModelMesh
{
public:
  ModelMesh(Mesh &&mesh, int materialIndex);

  Mesh mesh;
  Box bboxLocal;
  Sphere bsphereLocal;
  RenderData renderData;
  int materialIndex;
};

class HUSKY_DLL Model
{
public:
  static Model load(const std::string &filePath);

  Model();
  Model(Mesh &&mesh, const Material &mtl);

  int addMaterial(const Material &mtl);
  int addMesh(Mesh &&mesh, int mtlIndex);
  const Material& getMaterial(int mtlIndex) const;
  void draw(const Shader &shader, const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection) const;
  void calcBbox();

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

  void draw(const Shader &shader, const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection) const;

  const Model *model; // std::shared_ptr?
  int animationIndex;
  double animationTime;
};

}
