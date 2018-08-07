#include <husky/mesh/Model.hpp>
#include <husky/render/Texture.hpp>
#include <husky/util/SharedResource.hpp>
#include <husky/render/Shader.hpp>
#include <husky/Log.hpp>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>

namespace fs = std::experimental::filesystem;

namespace husky {

ModelNode::ModelNode(const std::string &name, const Matrix44d &mtxRelToParent, const ModelNode *parent)
  : name(name)
  , parent(parent)
  , children()
  , mtxRelToParent(mtxRelToParent)
  , mtxRelToModel(parent != nullptr ? (parent->mtxRelToModel * mtxRelToParent) : mtxRelToParent)
  , meshIndices()
{
}

ModelNode::~ModelNode()
{
  for (const ModelNode *child : children) {
    delete child;
  }
}

ModelMesh::ModelMesh(Mesh &&mesh, int materialIndex)
  : mesh(mesh)
  , renderData(mesh.getRenderData())
  , materialIndex(materialIndex)
{
  for (int iVert = 0; iVert < mesh.numVerts(); iVert++) {
    bboxLocal.expand(mesh.getPosition(iVert));
  }
}

static Matrix44f getAiMatrix(const aiMatrix4x4 &m)
{
  return {
    m.a1, m.b1, m.c1, m.d1,
    m.a2, m.b2, m.c2, m.d2,
    m.a3, m.b3, m.c3, m.d3,
    m.a4, m.b4, m.c4, m.d4
  };
}

static ModelNode* getAiNodesRecursive(const aiNode *node, ModelNode *parent)
{
  ModelNode *n = new ModelNode(node->mName.C_Str(), getAiMatrix(node->mTransformation), parent);

  for (unsigned int iMesh = 0; iMesh < node->mNumMeshes; iMesh++) {
    //Mesh m = meshes[node->mMeshes[iMesh]]; // Copy mesh before applying transform
    //m.transform(mat);
    //mdl.addRenderData(m.getRenderData(), meshMaterialIndices[iMesh]);

    n->meshIndices.emplace_back(node->mMeshes[iMesh]);
  }

  for (unsigned int iChild = 0; iChild < node->mNumChildren; iChild++) {
    ModelNode *c = getAiNodesRecursive(node->mChildren[iChild], n);
    n->children.emplace_back(c);
  }

  return n;
}

static Material getAiMaterial(const fs::path &folderPath, const aiMaterial *material)
{
  Material mtl;

  {
    aiString name;
    if (material->Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS) {
      mtl.name = name.C_Str();
    }
  }

  {
    aiColor3D diffuseColor;
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == aiReturn_SUCCESS) {
      mtl.diffuse.set(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    }
  }

  {
    aiColor3D specularColor;
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == aiReturn_SUCCESS) {
      mtl.specular.set(specularColor.r, specularColor.g, specularColor.b);
    }
  }

  {
    aiColor3D ambientColor;
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == aiReturn_SUCCESS) {
      mtl.ambient.set(ambientColor.r, ambientColor.g, ambientColor.b);
    }
  }

  {
    aiColor3D emissiveColor;
    if (material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) == aiReturn_SUCCESS) {
      mtl.emissive.set(emissiveColor.r, emissiveColor.g, emissiveColor.b);
    }
  }

  {
    float opacity;
    if (material->Get(AI_MATKEY_OPACITY, opacity) == aiReturn_SUCCESS) {
      mtl.opacity = opacity;
    }
  }

  {
    float shininess;
    if (material->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
      mtl.shininess = shininess;
    }
  }

  {
    float shininessStrength;
    if (material->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength) == aiReturn_SUCCESS) {
      mtl.shininessStrength = shininessStrength;
    }
  }

  {
    int wireframe;
    if (material->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe) == aiReturn_SUCCESS) {
      mtl.wireframe = (bool)wireframe;
    }
  }

  {
    int twoSided;
    if (material->Get(AI_MATKEY_TWOSIDED, twoSided) == aiReturn_SUCCESS) {
      mtl.twoSided = (bool)twoSided;
    }
  }

  {
    if (material->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE) > 0) {
      aiString textureFilePath;
      if (material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath) == aiReturn_SUCCESS) {
        auto p = fs::u8path(textureFilePath.C_Str());
        if (!p.is_absolute()) {
          p = folderPath / p;
        }
        auto img = SharedResource::loadImage(p.u8string());
        mtl.textureHandle = Texture::uploadTexture(*img);
      }
    }
  }

  return mtl;
}

static Mesh getAiMesh(const aiMesh *mesh)
{
  assert(mesh->HasPositions());

  Mesh m;

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
    int i = m.addBone(Bone(bone->mName.C_Str(), getAiMatrix(bone->mOffsetMatrix)));

    for (unsigned int iBoneWeight = 0; iBoneWeight < bone->mNumWeights; iBoneWeight++) {
      const aiVertexWeight &boneWeight = bone->mWeights[iBoneWeight];
      m.addBoneWeight((int)boneWeight.mVertexId, BoneWeight(i, boneWeight.mWeight));
    }
  }

  m.normalizeBoneWeights();

  return m;
}

static Animation getAiAnimation(const aiAnimation *anim)
{
  Animation animation(anim->mName.C_Str(), anim->mDuration, anim->mTicksPerSecond);

  for (unsigned int iChannel = 0; iChannel < anim->mNumChannels; iChannel++) {
    const aiNodeAnim *nodeAnim = anim->mChannels[iChannel];

    AnimationChannel animationChannel(nodeAnim->mNodeName.C_Str());

    for (unsigned int iKey = 0; iKey < nodeAnim->mNumPositionKeys; iKey++) {
      const auto &key = nodeAnim->mPositionKeys[iKey];
      animationChannel.keyframePosition[key.mTime] = { key.mValue.x, key.mValue.y, key.mValue.z };
    }

    for (unsigned int iKey = 0; iKey < nodeAnim->mNumRotationKeys; iKey++) {
      const auto &key = nodeAnim->mRotationKeys[iKey];
      animationChannel.keyframeRotation[key.mTime] = { key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w };
    }

    for (unsigned int iKey = 0; iKey < nodeAnim->mNumScalingKeys; iKey++) {
      const auto &key = nodeAnim->mScalingKeys[iKey];
      animationChannel.keyframeScale[key.mTime] = { key.mValue.x, key.mValue.y, key.mValue.z };
    }

    animation.channels.emplace(animationChannel.nodeName, std::move(animationChannel));
  }

  return animation;
}

Model Model::load(const std::string &filePath)
{
  const fs::path folderPath = fs::u8path(filePath).parent_path();

  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(filePath,
    aiProcess_CalcTangentSpace |
    aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices |
    aiProcess_SortByPType); // TODO: aiProcess_FlipUVs?

  if (scene == nullptr || scene->mRootNode == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
    Log::warning("Failed to load model: %s", filePath.c_str());
    return {};
  }

  //for (unsigned int iTex = 0; iTex < scene->mNumTextures; iTex++) {
  //  const aiTexture *tex = scene->mTextures[iTex];
  //  tex->
  //}

  Model mdl;

  // Get materials
  mdl.materials.reserve(scene->mNumMaterials);
  for (unsigned int iMtl = 0; iMtl < scene->mNumMaterials; iMtl++) {
    mdl.materials.emplace_back(getAiMaterial(folderPath, scene->mMaterials[iMtl]));
  }

  // Get meshes
  mdl.meshes.reserve(scene->mNumMeshes);
  for (unsigned int iMesh = 0; iMesh < scene->mNumMeshes; iMesh++) {
    mdl.addMesh(getAiMesh(scene->mMeshes[iMesh]), scene->mMeshes[iMesh]->mMaterialIndex);
  }

  // Get animations
  mdl.animations.reserve(scene->mNumAnimations);
  for (unsigned int iAnim = 0; iAnim < scene->mNumAnimations; iAnim++) {
    mdl.animations.emplace_back(getAiAnimation(scene->mAnimations[iAnim]));
  }

  mdl.root = getAiNodesRecursive(scene->mRootNode, nullptr);
  mdl.calcBbox();

  return mdl;
}

Model::Model()
  : root(nullptr)
{
}

Model::Model(Mesh &&mesh, const Material &mtl)
  : root(new ModelNode("Root", Matrix44d::identity(), nullptr))
{
  int iMtl = addMaterial(mtl);
  int iMesh = addMesh(std::move(mesh), iMtl);
  root->meshIndices.emplace_back(iMesh);
  calcBbox();
}

static void draw(const RenderData &renderData, const Shader &shader, const Material &mtl, const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection)
{
  if (shader.shaderProgramHandle == 0) {
    Log::warning("Invalid shader program");
    return;
  }

  if (mtl.twoSided) {
    glDisable(GL_CULL_FACE);
  }
  else {
    glEnable(GL_CULL_FACE);
  }

  glUseProgram(shader.shaderProgramHandle);

  int varLocation;

  if (shader.getUniformLocation("mtxModelView", varLocation)) {
    glUniformMatrix4fv(varLocation, 1, GL_FALSE, modelView.m);
  }

  const Matrix33f normalMatrix = modelView.get3x3();
  //const Matrix33f normalMatrix = modelView.inverted().transposed().get3x3(); // TODO: Use pre-inverted matrix for better performance
  //const Matrix33f normalMatrix = modelView.get3x3().inverted().transposed();
  if (shader.getUniformLocation("mtxNormal", varLocation)) {
    glUniformMatrix3fv(varLocation, 1, GL_FALSE, normalMatrix.m);
  }

  if (shader.getUniformLocation("mtxProjection", varLocation)) {
    glUniformMatrix4fv(varLocation, 1, GL_FALSE, projection.m);
  }

  if (shader.getUniformLocation("tex", varLocation)) {
    glUniform1i(varLocation, 0);
  }

  if (shader.getUniformLocation("lightDir", varLocation)) {
    Vector3f lightDir(20, -40, 100); // TODO
    lightDir = (view * Vector4f(lightDir, 0.0)).xyz.normalized();
    glUniform3fv(varLocation, 1, lightDir.val);
  }

  if (shader.getUniformLocation("mtlAmbient", varLocation)) {
    glUniform3fv(varLocation, 1, mtl.ambient.val);
  }

  if (shader.getUniformLocation("mtlDiffuse", varLocation)) {
    glUniform3fv(varLocation, 1, mtl.diffuse.val);
  }

  if (shader.getUniformLocation("mtlSpecular", varLocation)) {
    glUniform3fv(varLocation, 1, mtl.specular.val);
  }

  if (shader.getUniformLocation("mtlEmissive", varLocation)) {
    glUniform3fv(varLocation, 1, mtl.emissive.val);
  }

  if (shader.getUniformLocation("mtlShininess", varLocation)) {
    glUniform1f(varLocation, mtl.shininess);
  }

  if (shader.getUniformLocation("mtlShininessStrength", varLocation)) {
    glUniform1f(varLocation, mtl.shininessStrength);
  }

  if (shader.getUniformLocation("viewportSize", varLocation)) {
    glUniform2f(varLocation, (float)viewport.width, (float)viewport.height);
  }

  if (shader.getUniformLocation("lineWidth", varLocation)) {
    glUniform1f(varLocation, mtl.lineWidth);
  }

  if (mtl.textureHandle != 0) {
    //glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, mtl.textureHandle);
  }
  else {
    glBindTexture(GL_TEXTURE_2D, 0);
    //glDisable(GL_TEXTURE_2D);
  }

  if (renderData.vbo == 0) {
    Log::warning("renderData.vbo is 0");
    return;
  }

  if (renderData.vao == 0) {
    Log::warning("renderData.vao is 0");
    return;
  }

  glBindBuffer(GL_ARRAY_BUFFER, renderData.vbo);
  glBindVertexArray(renderData.vao);

  const void *attrPtr = nullptr;

  if (shader.getAttributeLocation("vertPosition", varLocation) && renderData.getAttribPointer(RenderData::Attribute::POSITION, attrPtr)) {
    glEnableVertexAttribArray(varLocation);
    glVertexAttribPointer(varLocation, 3, GL_FLOAT, GL_FALSE, renderData.vertByteCount, attrPtr);
  }

  if (shader.getAttributeLocation("vertNormal", varLocation) && renderData.getAttribPointer(RenderData::Attribute::NORMAL, attrPtr)) {
    glEnableVertexAttribArray(varLocation);
    glVertexAttribPointer(varLocation, 3, GL_FLOAT, GL_FALSE, renderData.vertByteCount, attrPtr);
  }

  if (shader.getAttributeLocation("vertTexCoord", varLocation) && renderData.getAttribPointer(RenderData::Attribute::TEXCOORD, attrPtr)) {
    glEnableVertexAttribArray(varLocation);
    glVertexAttribPointer(varLocation, 2, GL_FLOAT, GL_FALSE, renderData.vertByteCount, attrPtr);
  }

  if (shader.getAttributeLocation("vertColor", varLocation) && renderData.getAttribPointer(RenderData::Attribute::COLOR, attrPtr)) {
    glEnableVertexAttribArray(varLocation);
    glVertexAttribPointer(varLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, renderData.vertByteCount, attrPtr);
  }

  if (shader.getAttributeLocation("vertBoneIndices", varLocation) && renderData.getAttribPointer(RenderData::Attribute::BONE_INDICES, attrPtr)) {
    glEnableVertexAttribArray(varLocation);
    glVertexAttribPointer(varLocation, 4, GL_UNSIGNED_BYTE, GL_FALSE, renderData.vertByteCount, attrPtr);
  }

  if (shader.getAttributeLocation("vertBoneWeights", varLocation) && renderData.getAttribPointer(RenderData::Attribute::BONE_WEIGHTS, attrPtr)) {
    glEnableVertexAttribArray(varLocation);
    glVertexAttribPointer(varLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, renderData.vertByteCount, attrPtr);
  }

  GLenum mode = GL_POINTS; // Default fallback
  switch (renderData.mode) {
  case     RenderData::Mode::POINTS:    mode = GL_POINTS;          break;
  case     RenderData::Mode::LINES:     mode = GL_LINES;           break;
  case     RenderData::Mode::TRIANGLES: mode = GL_TRIANGLES;       break;
  default: Log::warning("Unsupported RenderData::Mode: %d", mode); break;
  }

  glDrawElements(mode, (int)renderData.indices.size(), GL_UNSIGNED_SHORT, renderData.indices.data());
}

int Model::addMaterial(const Material &mtl)
{
  materials.emplace_back(mtl);
  return (int)materials.size() - 1;
}

int Model::addMesh(Mesh &&mesh, int mtlIndex)
{
  meshes.emplace_back(ModelMesh(std::move(mesh), mtlIndex));
  return (int)meshes.size() - 1;
}

const Material& Model::getMaterial(int mtlIndex) const
{
  if (mtlIndex >= 0 && mtlIndex < materials.size()) {
    return materials[mtlIndex];
  }

  Log::warning("No material");
  static const Material fallbackMtl;
  return fallbackMtl;
}

// TODO: Remove? (Render ModelInstance, not Model)
void Model::draw(const Shader &shader, const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection) const
{
  for (const ModelNode *node : getNodesFlatList()) {
    for (int iMesh : node->meshIndices) {
      const ModelMesh &mesh = meshes[iMesh];
      const Material &mtl = getMaterial(mesh.materialIndex);
      husky::draw(mesh.renderData, shader, mtl, viewport, view, modelView * (Matrix44f)node->mtxRelToModel, projection);
    }
  }
}

void Model::calcBbox()
{
  bboxLocal = {};

  // TODO: Take node transformation and mesh usage into consideration
  for (const auto &mm : meshes) {
    if (mm.bboxLocal.initialized) {
      bboxLocal.expand(mm.bboxLocal.min);
      bboxLocal.expand(mm.bboxLocal.max);
    }
  }
}

std::vector<const ModelNode*> Model::getNodesFlatList() const
{
  std::vector<const ModelNode*> nodes;
  getNodesRecursive(root, nodes);
  return nodes;
}

void Model::getNodesRecursive(const ModelNode *node, std::vector<const ModelNode*> &nodes) const
{
  if (node != nullptr) {
    nodes.emplace_back(node);

    for (const ModelNode *child : node->children) {
      getNodesRecursive(child, nodes);
    }
  }
}

ModelInstance::ModelInstance(Model *model)
  : model(model)
  , animationIndex(-1)
  , animationTime(0)
{
  assert(model != nullptr);
  if (!model->animations.empty()) { animationIndex = 0; } // TODO: Remove
}

void ModelInstance::draw(const Shader &shader, const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection) const
{
  //glUseProgram(shader.shaderProgram);

  //int varLocation;

  //if (shader.getUniformLocation("mtxBones", varLocation)) {
  //  // TODO
  //  const std::vector<Matrix44f> mtxBones = modelInstance.getAnimatedBoneMatrices();
  //  if (!mtxBones.empty()) {
  //    glUniformMatrix4fv(varLocation, mtxBones.size(), GL_FALSE, mtxBones.front().m);
  //  }
  //}

  if (model != nullptr) {
    model->draw(shader, viewport, view, modelView, projection);
  }
}

}
