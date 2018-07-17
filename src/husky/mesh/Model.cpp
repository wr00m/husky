#include <husky/mesh/Model.hpp>
#include <husky/Log.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace husky {

static void getNodeMeshesRecursive(const aiNode *node, const Matrix44d *matParent, Model &mdl)
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
    SimpleMesh m = mdl.meshes[node->mMeshes[iMesh]]; // Copy before applying transform
    m.transform(mat);
    mdl.transformedMeshes.emplace_back(m);
  }

  for (unsigned int iChild = 0; iChild < node->mNumChildren; iChild++) {
    getNodeMeshesRecursive(node->mChildren[iChild], &mat, mdl);
  }
}

static Material getMaterial(const aiMaterial *material)
{
  Material mtl;

  {
    aiString name;
    aiGetMaterialString(material, AI_MATKEY_NAME, &name);
    mtl.name = name.C_Str();
    //Log::info("mtl.name: %s", mtl.name.c_str());
  }

  {
    aiColor4D diffuseColor(1.f, 1.f, 1.f, 1.f);
    aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);
    mtl.diffuseColor.set(std::uint8_t(diffuseColor.r * 255), std::uint8_t(diffuseColor.g * 255), std::uint8_t(diffuseColor.b * 255));
    //Log::info("mtl.diffuseColor: %d %d %d", mtl.diffuseColor.r, mtl.diffuseColor.g, mtl.diffuseColor.b);
  }

  {
    aiColor4D specularColor(1.f, 1.f, 1.f, 1.f);
    aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specularColor);
    mtl.specularColor.set(std::uint8_t(specularColor.r * 255), std::uint8_t(specularColor.g * 255), std::uint8_t(specularColor.b * 255));
    //Log::info("mtl.specularColor: %d %d %d", mtl.specularColor.r, mtl.specularColor.g, mtl.specularColor.b);
  }

  {
    aiColor4D ambientColor(1.f, 1.f, 1.f, 1.f);
    aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambientColor);
    mtl.ambientColor.set(std::uint8_t(ambientColor.r * 255), std::uint8_t(ambientColor.g * 255), std::uint8_t(ambientColor.b * 255));
    //Log::info("mtl.ambientColor: %d %d %d", mtl.ambientColor.r, mtl.ambientColor.g, mtl.ambientColor.b);
  }

  {
    aiColor4D emissiveColor(0.f, 0.f, 0.f, 1.f);
    aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &emissiveColor);
    mtl.emissiveColor.set(std::uint8_t(emissiveColor.r * 255), std::uint8_t(emissiveColor.g * 255), std::uint8_t(emissiveColor.b * 255));
    //Log::info("mtl.emissiveColor: %d %d %d", mtl.emissiveColor.r, mtl.emissiveColor.g, mtl.emissiveColor.b);
  }

  {
    float opacity = 1.f;
    aiGetMaterialFloat(material, AI_MATKEY_OPACITY, &opacity);
    mtl.opacity = std::uint8_t(opacity * 255);
    //Log::info("mtl.opacity: %d", mtl.opacity);
  }

  {
    float shininess = 1.f;
    aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);
    mtl.shininess = std::uint8_t(shininess * 255);
    //Log::info("mtl.shininess: %d", mtl.shininess);
  }

  {
    float shininessStrength = 1.f;
    aiGetMaterialFloat(material, AI_MATKEY_SHININESS_STRENGTH, &shininessStrength);
    mtl.shininessStrength = std::uint8_t(shininessStrength * 255);
    //Log::info("mtl.shininessStrength: %d", mtl.shininessStrength);
  }

  {
    int wireframe = 0;
    aiGetMaterialInteger(material, AI_MATKEY_ENABLE_WIREFRAME, &wireframe);
    mtl.wireframe = (bool)wireframe;
    //Log::info("mtl.wireframe: %d", mtl.wireframe);
  }

  {
    int twoSided = 0;
    aiGetMaterialInteger(material, AI_MATKEY_TWOSIDED, &twoSided);
    mtl.twoSided = (bool)twoSided;
    //Log::info("mtl.twoSided: %d", mtl.twoSided);
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
    SimpleMesh::Vertex v({ pos.x, pos.y, pos.z });

    if (mesh->HasNormals()) {
      const aiVector3D &normal = mesh->mNormals[iVert];
      v.normal.set(normal.x, normal.y, normal.z);
    }

    if (mesh->HasTextureCoords(0)) {
      const aiVector3D &uv = mesh->mTextureCoords[0][iVert];
      v.texCoord.set(uv.x, uv.y);
    }

    if (mesh->HasVertexColors(0)) {
      const aiColor4D &color = mesh->mColors[0][iVert];
      v.color.set(std::uint8_t(color.r * 255), std::uint8_t(color.g * 255), std::uint8_t(color.b * 255), std::uint8_t(color.a * 255));
    }

    m.addVertex(v);
  }

  for (unsigned int iFace = 0; iFace < mesh->mNumFaces; iFace++) {
    const aiFace &face = mesh->mFaces[iFace];
    assert(face.mNumIndices == 3); // aiProcess_Triangulate
    m.addTriangle(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
    //m.addTriangle(face.mIndices[0], face.mIndices[2], face.mIndices[1]);
  }

  for (unsigned int iBone = 0; iBone < mesh->mNumBones; iBone++) {
    const aiBone *bone = mesh->mBones[iBone];
    Log::info("Bone: %s", bone->mName.C_Str());
  }

  return m;
}

Model Model::load(const std::string &filePath)
{
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(filePath,
    aiProcess_CalcTangentSpace |
    aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices |
    aiProcess_SortByPType);

  if (scene == nullptr) {
    Log::warning("Failed to load model: %s", filePath.c_str());
    return {};
  }

  Model mdl;
  mdl.materials.reserve(scene->mNumMaterials);
  mdl.meshes.reserve(scene->mNumMeshes);

  for (unsigned int iMtl = 0; iMtl < scene->mNumMaterials; iMtl++) {
    mdl.materials.emplace_back(getMaterial(scene->mMaterials[iMtl]));
  }

  for (unsigned int iMesh = 0; iMesh < scene->mNumMeshes; iMesh++) {
    mdl.meshes.emplace_back(getMesh(scene->mMeshes[iMesh]));
  }

  //for (unsigned int iAnim = 0; iAnim < scene->mNumAnimations; iAnim++) {
  //  const aiAnimation *anim = scene->mAnimations[iAnim];
  //  Log::info("Animation: %s", anim->mName.C_Str());
  //}

  getNodeMeshesRecursive(scene->mRootNode, nullptr, mdl);
  return mdl;
}

Model::Model()
{
}

Model::Model(const SimpleMesh &&mesh)
{
  meshes.emplace_back(mesh);
  transformedMeshes.emplace_back(meshes.back());
}

}
