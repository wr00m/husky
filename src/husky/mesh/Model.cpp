#include <husky/mesh/Model.hpp>
#include <husky/Log.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace husky {

void Model::load(const std::string &filePath)
{
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(filePath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
  //importer.ReadFileFromMemory()

  if (scene == nullptr) {
    Log::warning("Failed to load model: %s", filePath.c_str());
  }

  // TODO
}

}
