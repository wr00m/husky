#pragma once

#include <husky/mesh/SimpleMesh.hpp>
#include <husky/mesh/Material.hpp>
#include <husky/mesh/Animation.hpp>
#include <string>

namespace husky {

class HUSKY_DLL Model
{
public:
  static Model load(const std::string &filePath);

  Model();
  Model(const RenderData &&renderData, const Material &mtl);
  Model(const SimpleMesh &mesh, const Material &mtl);

  void addRenderData(const RenderData &&renderData, int mtlIndex);
  void addRenderData(const RenderData &&renderData, const Material &mtl);

  std::vector<Material> materials;
  std::vector<RenderData> meshRenderDatas;
  std::vector<int> meshMaterialIndices;
  std::vector<Animation> animations;
};

}
