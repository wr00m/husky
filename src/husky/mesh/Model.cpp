#include <husky/mesh/Model.hpp>
#include <husky/Log.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace husky {

static void getNodeMeshesRecursive(const aiNode *node, const Matrix44d *matParent, const std::vector<SimpleMesh> &meshes, const std::vector<unsigned int> &meshMaterialIndices, Model &mdl)
{
  const aiMatrix4x4 &t = node->mTransformation;
  Matrix44d mat(
    t.a1, t.b1, t.c1, t.d1,
    t.a2, t.b2, t.c2, t.d2,
    t.a3, t.b3, t.c3, t.d3,
    t.a4, t.b4, t.c4, t.d4);
  //mat.transpose();

  if (matParent != nullptr) {
    mat = (*matParent) * mat;
  }

  for (unsigned int iMesh = 0; iMesh < node->mNumMeshes; iMesh++) {
    SimpleMesh m = meshes[node->mMeshes[iMesh]]; // Copy mesh before applying transform
    m.transform(mat);
    mdl.addRenderData(m.getRenderData(), meshMaterialIndices[iMesh]);
  }

  for (unsigned int iChild = 0; iChild < node->mNumChildren; iChild++) {
    getNodeMeshesRecursive(node->mChildren[iChild], &mat, meshes, meshMaterialIndices, mdl);
  }
}

static Material getMaterial(const aiMaterial *material)
{
  Material mtl;

  {
    aiString name;
    if (material->Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS) {
      mtl.name = name.C_Str();
      //Log::info("mtl.name: %s", mtl.name.c_str());
    }
  }

  {
    aiColor3D diffuseColor;
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == aiReturn_SUCCESS) {
      mtl.diffuse.set(diffuseColor.r, diffuseColor.g, diffuseColor.b);
      //Log::info("mtl.diffuseColor: %f %f %f", mtl.diffuseColor.r, mtl.diffuseColor.g, mtl.diffuseColor.b);
    }
  }

  {
    aiColor3D specularColor;
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == aiReturn_SUCCESS) {
      mtl.specular.set(specularColor.r, specularColor.g, specularColor.b);
      //Log::info("mtl.specularColor: %f %f %f", mtl.specularColor.r, mtl.specularColor.g, mtl.specularColor.b);
    }
  }

  {
    aiColor3D ambientColor;
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == aiReturn_SUCCESS) {
      mtl.ambient.set(ambientColor.r, ambientColor.g, ambientColor.b);
      //Log::info("mtl.ambientColor: %f %f %f", mtl.ambientColor.r, mtl.ambientColor.g, mtl.ambientColor.b);
    }
  }

  {
    aiColor3D emissiveColor;
    if (material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) == aiReturn_SUCCESS) {
      mtl.emissive.set(emissiveColor.r, emissiveColor.g, emissiveColor.b);
      //Log::info("mtl.emissiveColor: %f %f %f", mtl.emissiveColor.r, mtl.emissiveColor.g, mtl.emissiveColor.b);
    }
  }

  {
    float opacity;
    if (material->Get(AI_MATKEY_OPACITY, opacity) == aiReturn_SUCCESS) {
      mtl.opacity = opacity;
      //Log::info("mtl.opacity: %f", mtl.opacity);
    }
  }

  {
    float shininess;
    if (material->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
      mtl.shininess = shininess;
      //Log::info("mtl.shininess: %f", mtl.shininess);
    }
  }

  {
    float shininessStrength;
    if (material->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength) == aiReturn_SUCCESS) {
      mtl.shininessStrength = shininessStrength;
      //Log::info("mtl.shininessStrength: %f", mtl.shininessStrength);
    }
  }

  {
    int wireframe;
    if (material->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe) == aiReturn_SUCCESS) {
      mtl.wireframe = (bool)wireframe;
      //Log::info("mtl.wireframe: %d", mtl.wireframe);
    }
  }

  {
    int twoSided;
    if (material->Get(AI_MATKEY_TWOSIDED, twoSided) == aiReturn_SUCCESS) {
      mtl.twoSided = (bool)twoSided;
      //Log::info("mtl.twoSided: %d", mtl.twoSided);
    }
  }

  return mtl;
}

static SimpleMesh getMesh(const aiMesh *mesh)
{
  assert(mesh->HasPositions());

  SimpleMesh m;

  for (unsigned int iVert = 0; iVert < mesh->mNumVertices; iVert++)
  {
    const aiVector3D &pos = mesh->mVertices[iVert];
    int i = m.addVert({ pos.x, pos.y, pos.z });

    if (mesh->HasNormals()) {
      const aiVector3D &normal = mesh->mNormals[iVert];
      m.setNormal(i, { normal.x, normal.y, normal.z });
    }

    if (mesh->HasTextureCoords(0)) {
      const aiVector3D &uv = mesh->mTextureCoords[0][iVert];
      m.setTexCoord(i, { uv.x, uv.y });
    }

    if (mesh->HasVertexColors(0)) {
      const aiColor4D &color = mesh->mColors[0][iVert];
      m.setColor(i, { std::uint8_t(color.r * 255), std::uint8_t(color.g * 255), std::uint8_t(color.b * 255), std::uint8_t(color.a * 255) });
    }
  }

  for (unsigned int iFace = 0; iFace < mesh->mNumFaces; iFace++) {
    const aiFace &face = mesh->mFaces[iFace];
    assert(face.mNumIndices == 3); // We have requested triangulated faces
    m.addTriangle({ (int)face.mIndices[0], (int)face.mIndices[1], (int)face.mIndices[2] });
  }

  for (unsigned int iBone = 0; iBone < mesh->mNumBones; iBone++) {
    const aiBone *bone = mesh->mBones[iBone];
    int i = m.addBone({ bone->mName.C_Str(), -1, Matrix44f::identity() });

    for (unsigned int iBoneWeight = 0; iBoneWeight < bone->mNumWeights; iBoneWeight++) {
      const aiVertexWeight &boneWeight = bone->mWeights[iBoneWeight];
      m.addBoneWeight((int)boneWeight.mVertexId, { i, boneWeight.mWeight });
    }
  }

  m.normalizeBoneWeights();

  return m;
}

Model Model::load(const std::string &filePath)
{
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(filePath,
    aiProcess_CalcTangentSpace |
    aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices |
    aiProcess_SortByPType); // TODO: aiProcess_FlipUVs?

  if (scene == nullptr) {
    Log::warning("Failed to load model: %s", filePath.c_str());
    return {};
  }

  Model mdl;
  mdl.materials.reserve(scene->mNumMaterials);
  for (unsigned int iMtl = 0; iMtl < scene->mNumMaterials; iMtl++) {
    mdl.materials.emplace_back(getMaterial(scene->mMaterials[iMtl]));
  }

  std::vector<SimpleMesh> meshes;
  std::vector<unsigned int> meshMaterialIndices;
  meshes.reserve(scene->mNumMeshes);
  for (unsigned int iMesh = 0; iMesh < scene->mNumMeshes; iMesh++) {
    meshes.emplace_back(getMesh(scene->mMeshes[iMesh]));
    meshMaterialIndices.emplace_back(scene->mMeshes[iMesh]->mMaterialIndex);
  }

  //for (unsigned int iAnim = 0; iAnim < scene->mNumAnimations; iAnim++) {
  //  const aiAnimation *anim = scene->mAnimations[iAnim];
  //  Log::info("Animation: %s", anim->mName.C_Str());
  //}

  getNodeMeshesRecursive(scene->mRootNode, nullptr, meshes, meshMaterialIndices, mdl);
  return mdl;
}

Model::Model()
{
}

Model::Model(const RenderData &&renderData, const Material &mtl)
{
  addRenderData(std::move(renderData), mtl);
}

Model::Model(const SimpleMesh &mesh, const Material &mtl)
  : Model(std::move(mesh.getRenderData()), mtl)
{
}

void Model::addRenderData(const RenderData &&renderData, int mtlIndex)
{
  meshRenderDatas.emplace_back(renderData);
  meshMaterialIndices.emplace_back(mtlIndex);
}

void Model::addRenderData(const RenderData &&renderData, const Material &mtl)
{
  int mtlIndex = (int)materials.size();
  materials.emplace_back(mtl);
  addRenderData(std::move(renderData), mtlIndex);
}

}
