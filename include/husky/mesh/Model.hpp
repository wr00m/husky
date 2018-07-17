#pragma once

#include <husky/mesh/SimpleMesh.hpp>
#include <husky/mesh/Material.hpp>
#include <string>

namespace husky {

class HUSKY_DLL Model
{
public:
  static Model load(const std::string &filePath);

  Model();
  Model(RenderData &&renderData);
  Model(const SimpleMesh &mesh);

#pragma warning(suppress: 4251)
  std::vector<Material> materials;
#pragma warning(suppress: 4251)
  std::vector<RenderData> meshRenderDatas;
#pragma warning(suppress: 4251)
  std::vector<int> meshMaterialIndices;
};

}
