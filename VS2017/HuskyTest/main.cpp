#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <husky/Log.hpp>
#include <husky/image/Image.hpp>
#include <husky/math/Matrix44.hpp>
#include <husky/math/Random.hpp>
#include <husky/math/Quaternion.hpp>
#include <husky/math/MathUtil.hpp>
#include <husky/mesh/Primitive.hpp>
#include <husky/render/Camera.hpp>
#include <husky/render/Viewport.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>

static double matDiff(const husky::Matrix44d &a, const glm::dmat4x4 &g)
{
  std::ostringstream ossA, ossG;
  ossA.precision(2);
  ossG.precision(2);

  double diffSumSq = 0.0;
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      ossA << std::fixed << std::setw(5) << a[col][row] << (col == 3 ? "\n" : " ");
      ossG << std::fixed << std::setw(5) << g[col][row] << (col == 3 ? "\n" : " ");
      double elementDiff = a[col][row] - g[col][row];
      diffSumSq += elementDiff * elementDiff;
    }
  }

  //std::cout << ossA.str() << std::endl << ossG.str() << std::endl << std::endl;

  return diffSumSq;
}

static double quatDiff(const husky::Quaterniond &a, const glm::dquat &g)
{
  std::ostringstream ossA, ossG;
  ossA.precision(2);
  ossG.precision(2);

  ossA << std::fixed << std::setw(5) << a.x << " " << a.y << " " << a.z << " " << a.w << " " << std::endl;
  ossG << std::fixed << std::setw(5) << g.x << " " << g.y << " " << g.z << " " << g.w << " " << std::endl;
  husky::Vector4d diff(a.x - g.x, a.y - g.y, a.z - g.z, a.w - g.w);
  double diffSumSq = diff.dot(diff);

  //std::cout << ossA.str() << std::endl << ossG.str() << std::endl << std::endl;

  return diffSumSq;
}

static void runUnitTests() // TODO: Remove GLM; use explicit expected matrices
{
  husky::Matrix44d lookAt      = husky::Matrix44d::lookAt({ 2.0, 8.0, 4.0 }, { -5.0, 6.0, 7.0 }, { 0.0, 1.0, 1.0 });
  glm::dmat4x4     lookAtGlm   = glm::lookAt(glm::dvec3   { 2.0, 8.0, 4.0 }, { -5.0, 6.0, 7.0 }, { 0.0, 1.0, 1.0 });
  double           lookAtDiff  = matDiff(lookAt, lookAtGlm);
  assert(lookAtDiff < 1e-9);

  husky::Matrix44d ortho       = husky::Matrix44d::ortho(0.0, 1.0, 2.0, 3.0, 4.0, 5.0);
  glm::dmat4x4     orthoGlm    =              glm::ortho(0.0, 1.0, 2.0, 3.0, 4.0, 5.0);
  double           orthoDiff   = matDiff(ortho, orthoGlm);
  assert(orthoDiff < 1e-9);

  husky::Matrix44d frustum     = husky::Matrix44d::frustum(0.0, 1.0, 2.0, 3.0, 4.0, 5.0);
  glm::dmat4x4     frustumGlm  =              glm::frustum(0.0, 1.0, 2.0, 3.0, 4.0, 5.0);
  double           frustumDiff = matDiff(frustum, frustumGlm);
  assert(frustumDiff < 1e-9);

  husky::Matrix44d persp       = husky::Matrix44d::perspective(husky::math::deg2rad * 60.0, 1.5, 1.0, 100.0);
  glm::dmat4x4     perspGlm    = glm::perspective(husky::math::deg2rad * 60.0, 1.5, 1.0, 100.0);
  double           perspDiff   = matDiff(persp, perspGlm);
  assert(perspDiff < 1e-9);

  husky::Matrix44d rot         = husky::Matrix44d::rotate(33.0, { 12.0, 13.0, 14.0 });
  glm::dmat4x4     rotGlm      = glm::rotate(33.0, glm::dvec3{ 12.0, 13.0, 14.0 });
  double           rotDiff     = matDiff(rot, rotGlm);
  assert(rotDiff < 1e-9);

  husky::Matrix44d mul = lookAt * persp;
  glm::dmat4x4 mulGlm = lookAtGlm * perspGlm;
  double mulDiff = matDiff(mul, mulGlm);
  assert(mulDiff < 1e-9);

  husky::Quaterniond quatAxisAngle = husky::Quaterniond::fromAxisAngle(33.0, husky::Vector3d(12.0, 13.0, 14.0).normalized());
  glm::dquat quatAxisAngleGlm = glm::angleAxis(33.0, glm::normalize(glm::dvec3{ 12.0, 13.0, 14.0 }));
  double quatAxisAngleDiff = quatDiff(quatAxisAngle, quatAxisAngleGlm);
  assert(quatAxisAngleDiff < 1e-9);

  husky::Matrix44d matFromQuat = quatAxisAngle.toMatrix();
  glm::dmat4x4 matFromQuatGlm(quatAxisAngleGlm);
  double matFromQuatDiff = matDiff(matFromQuat, matFromQuatGlm);
  assert(matFromQuatDiff < 1e-9);

  husky::Quaterniond quatDirs = husky::Quaterniond::fromDirections(husky::Vector3d(1, -2, 3).normalized(), husky::Vector3d(-4, 5, -6).normalized());
  glm::dquat quatDirsGlm = glm::rotation(glm::normalize(glm::dvec3(1, -2, 3)), glm::normalize(glm::dvec3(-4, 5, -6)));
  double quatDirsDiff = quatDiff(quatDirs, quatDirsGlm);
  assert(quatDirsDiff < 1e-9);

  // TODO: Investigate why Quaternion::fromRotationMatrix() differs from GLM

  husky::Matrix44d perspInf = husky::Matrix44d::perspectiveInf(husky::math::pi / 3.0, 1.25, 0.1);
  glm::dmat4x4 perspInfGlm = glm::infinitePerspective(husky::math::pi / 3.0, 1.25, 0.1);
  double perspInfDiff = matDiff(perspInf, perspInfGlm);
  assert(perspInfDiff < 1e-9);

  husky::Matrix44d perspInfTweak = husky::Matrix44d::perspectiveInf(husky::math::pi / 3.0, 1.25, 0.1, 1e-4);
  glm::dmat4x4 perspInfTweakGlm = glm::tweakedInfinitePerspective(husky::math::pi / 3.0, 1.25, 0.1, 1e-4);
  double perspInfTweakDiff = matDiff(perspInfTweak, perspInfTweakGlm);
  assert(perspInfTweakDiff < 1e-9);

  //husky::Matrix44d perspInfRevZ = husky::Matrix44d::perspectiveInfRevZ(husky::math::pi / 3.0, 1.25, 0.1);

  double vec3Angle = husky::Vector3d(1, 0, 0).angleAbs({ 0, 1, 0 });
  double vec3AngleGlm = glm::angle(glm::dvec3(1, 0, 0), glm::dvec3(0, 1, 0));
  double vec3AngleDiff = std::abs(vec3Angle - vec3AngleGlm);
  assert(vec3AngleDiff < 1e-9);

  double vec2Angle = husky::Vector2d(1, 0).angleSigned({ 0, -1 });
  double vec2AngleGlm = glm::orientedAngle(glm::dvec2(1, 0), glm::dvec2(0, -1));
  double vec2AngleDiff = std::abs(vec2Angle - vec2AngleGlm);
  assert(vec2AngleDiff < 1e-9);

  //double quatAngle = ;
  //double quatAngleGlm = ;
  //double quatAngleDiff = std::abs(quatAngle - quatAngleGlm);
  //assert(quatAngleDiff < 1e-9);
}

static const char *vertShaderSrc =
R"(#version 400
uniform mat4 modelView;
uniform mat4 projection;
in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec4 vColor;
out vec3 varNormal;
out vec2 varTexCoord;
out vec4 varColor;
void main() {
  varNormal = vNormal;
  varTexCoord = vTexCoord;
  varColor = vColor;
  gl_Position = projection * (modelView * vec4(vPosition, 1.0));
})";

static const char *fragShaderSrc =
R"(#version 400
uniform sampler2D tex;
uniform vec3 lightDir = normalize(vec3(1.0, 4.0, 10.0));
uniform vec3 ambientColor = vec3(0.05, 0.05, 0.05);
in vec3 varNormal;
in vec2 varTexCoord;
in vec4 varColor;
out vec4 fragColor;
void main() {
  //vec3 normal = normalize(varNormal);
  float light = dot(varNormal, lightDir);
  light = clamp(light, 0.0, 1.0);
  vec4 texColor = texture(tex, varTexCoord);
  fragColor = vec4(light * varColor.rgb * texColor.rgb + ambientColor, varColor.a * texColor.a);
  //fragColor = texColor;
  //fragColor = vec4(varTexCoord, 0.0, 1.0);
})";

static void messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
  switch (severity) {
  case GL_DEBUG_SEVERITY_LOW:
  case GL_DEBUG_SEVERITY_MEDIUM:
  case GL_DEBUG_SEVERITY_HIGH:
    husky::Log::warning("OpenGL callback: %s", message);
    break;

  case GL_DEBUG_SEVERITY_NOTIFICATION:
  default:
    // Ignore
    break;
  }
}

static husky::Camera cam({ 0, -20, 5 }, {});
static GLFWwindow *window = nullptr;
static husky::Vector2d prevMousePos(0, 0);
static husky::Vector2i windowedPos(0, 0);
static husky::Vector2i windowedSize(1280, 720);
static husky::Viewport viewport;
static double prevTime = 0.0;
static double frameTime = (1.0 / 60.0);
static bool mouseDragRight = false;

static void toggleFullscreen(GLFWwindow *win)
{
  bool fullscreen = (glfwGetWindowMonitor(win) != nullptr);
  if (fullscreen) {
    glfwSetWindowMonitor(win, nullptr, windowedPos.x, windowedPos.y, windowedSize.x, windowedSize.y, 0);
  }
  else {
    // Remember windowed position and size
    glfwGetWindowPos(win, &windowedPos.x, &windowedPos.y);
    glfwGetWindowSize(win, &windowedSize.x, &windowedSize.y);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    glfwSetWindowMonitor(win, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
  }
}

static void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_F11) {
      toggleFullscreen(win);
    }
    else if (key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(win, GLFW_TRUE);
    }
  }
}

static void mouseMoveCallback(GLFWwindow* win, double x, double y)
{
  husky::Vector2d mousePos(x, y);
  husky::Vector2d mouseDelta = mousePos - prevMousePos;

  //if (mousePos.x < 0 || mousePos.y < 0) {
  //  std::cout << "pos   : " << mousePos.x << " " << mousePos.y << std::endl;
  //  std::cout << "delta : " << mouseDelta.x << " " << mouseDelta.y << std::endl;
  //}

  if (mouseDragRight) {
    const double rotSpeed = 0.01;
    cam.attitude = husky::Quaterniond::fromAxisAngle(rotSpeed * -mouseDelta.x, { 0, 0, 1 }) * cam.attitude; // Yaw
    cam.attitude = husky::Quaterniond::fromAxisAngle(rotSpeed * -mouseDelta.y, cam.right()) * cam.attitude; // Pitch
  }

  prevMousePos = mousePos;
}

static void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods)
{
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      mouseDragRight = true;
    }
  }
  else if (action == GLFW_RELEASE) {
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    mouseDragRight = false;
  }
}

static void scrollCallback(GLFWwindow *win, double deltaX, double deltaY)
{
  constexpr double zoomSpeed = 1.0;
  cam.position += cam.forward() * zoomSpeed * deltaY;
}

static void handleInput(GLFWwindow *win)
{
  husky::Vector3d input;
  input.x = (glfwGetKey(win, GLFW_KEY_D) - glfwGetKey(win, GLFW_KEY_A));
  input.y = (glfwGetKey(win, GLFW_KEY_W) - glfwGetKey(win, GLFW_KEY_S));
  input.z = (glfwGetKey(win, GLFW_KEY_SPACE) - glfwGetKey(win, GLFW_KEY_LEFT_CONTROL));

  const husky::Vector3d camSpeed(20, 20, 20);

  cam.position += cam.right()   * input.x * camSpeed.x * frameTime;
  cam.position += cam.forward() * input.y * camSpeed.y * frameTime;
  cam.position += cam.up()      * input.z * camSpeed.z * frameTime;
}

class Material
{
public:
  Material(GLuint shaderProgram, GLuint tex)
    : shaderProgram(shaderProgram)
    , tex(tex)
  {
    modelViewLocation   = glGetUniformLocation(shaderProgram, "modelView");
    projectionLocation  = glGetUniformLocation(shaderProgram, "projection");
    texLocation         = glGetUniformLocation(shaderProgram, "tex");

    vertPositionLocation  = glGetAttribLocation(shaderProgram, "vPosition");
    vertNormalLocation    = glGetAttribLocation(shaderProgram, "vNormal");
    vertTexCoordLocation  = glGetAttribLocation(shaderProgram, "vTexCoord");
    vertColorLocation     = glGetAttribLocation(shaderProgram, "vColor");
  }

  GLuint shaderProgram;
  GLint modelViewLocation;
  GLint projectionLocation;
  GLint texLocation;
  GLint vertPositionLocation;
  GLint vertNormalLocation;
  GLint vertTexCoordLocation;
  GLint vertColorLocation;
  GLuint tex;
};

class Entity
{
private:
  static void draw(const Material &mtl, const husky::RenderData &renderData, const husky::Matrix44f &modelView, const husky::Matrix44f &projection, GLuint vbo, GLuint vao)
  {
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, mtl.tex);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);

    if (mtl.vertPositionLocation != -1) {
      glEnableVertexAttribArray(mtl.vertPositionLocation);
      glVertexAttribPointer(mtl.vertPositionLocation, 3, GL_FLOAT, GL_FALSE, renderData.vertByteCount, renderData.attribPointer(husky::RenderData::Attribute::POSITION));
    }

    if (mtl.vertNormalLocation != -1) {
      glEnableVertexAttribArray(mtl.vertNormalLocation);
      glVertexAttribPointer(mtl.vertNormalLocation, 3, GL_FLOAT, GL_FALSE, renderData.vertByteCount, renderData.attribPointer(husky::RenderData::Attribute::NORMAL));
    }

    if (mtl.vertTexCoordLocation != -1) {
      glEnableVertexAttribArray(mtl.vertTexCoordLocation);
      glVertexAttribPointer(mtl.vertTexCoordLocation, 2, GL_FLOAT, GL_FALSE, renderData.vertByteCount, renderData.attribPointer(husky::RenderData::Attribute::TEXCOORD));
    }

    if (mtl.vertColorLocation != -1) {
      glEnableVertexAttribArray(mtl.vertColorLocation);
      glVertexAttribPointer(mtl.vertColorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, renderData.vertByteCount, renderData.attribPointer(husky::RenderData::Attribute::COLOR));
    }

    glUseProgram(mtl.shaderProgram);

    if (mtl.modelViewLocation != -1) {
      glUniformMatrix4fv(mtl.modelViewLocation, 1, GL_FALSE, modelView.m);
    }

    if (mtl.projectionLocation != -1) {
      glUniformMatrix4fv(mtl.projectionLocation, 1, GL_FALSE, projection.m);
    }

    if (mtl.texLocation != -1) {
      glUniform1i(mtl.texLocation, 0);
    }

    GLenum mode = GL_POINTS; // Default fallback;
    switch (renderData.mode) {
    case     husky::RenderData::Mode::POINTS:    mode = GL_POINTS;    break;
    case     husky::RenderData::Mode::LINES:     mode = GL_LINES;     break;
    case     husky::RenderData::Mode::TRIANGLES: mode = GL_TRIANGLES; break;
    default: husky::Log::warning("Unsupported RenderData::Mode");     break;
    }

    glDrawElements(mode, (int)renderData.indices.size(), GL_UNSIGNED_SHORT, renderData.indices.data());
  }

public:
  Entity(const Material &material, const husky::SimpleMesh &mesh)
    : material(material)
  {
    renderData = mesh.getRenderData();

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, renderData.bytes.size(), renderData.bytes.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);


    bboxLocal = mesh.getBoundingBox();
    husky::Vector3d bboxSize = bboxLocal.size();
    husky::SimpleMesh bboxMesh = husky::Primitive::box(bboxSize.x, bboxSize.y, bboxSize.z);
    bboxMesh.transform(husky::Matrix44d::translate(bboxLocal.center()));
    bboxRenderData = bboxMesh.getRenderDataWireframe();

    glGenBuffers(1, &vboBbox);
    glBindBuffer(GL_ARRAY_BUFFER, vboBbox);
    glBufferData(GL_ARRAY_BUFFER, bboxRenderData.bytes.size(), bboxRenderData.bytes.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vaoBbox);
    //glBindVertexArray(vaoBbox);
  }

  void draw(const husky::Camera &cam) const
  {
    const husky::Matrix44f modelView(cam.view * transform);
    const husky::Matrix44f projection(cam.projection);

    draw(material, renderData, modelView, projection, vbo, vao);
    draw(material, bboxRenderData, modelView, projection, vboBbox, vaoBbox);
  }

  husky::Matrix44d transform = husky::Matrix44d::identity();
  Material material;
  husky::RenderData renderData;
  husky::BoundingBox bboxLocal;
  //husky::BoundingBox bboxWorld;
  husky::RenderData bboxRenderData;

private:
  GLuint vbo;
  GLuint vao;
  GLuint vboBbox;
  GLuint vaoBbox;
};

int main()
{
  runUnitTests();

  if (!glfwInit()) {
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

  window = glfwCreateWindow(windowedSize.x, windowedSize.y, "Hello Husky!", NULL, NULL);
  if (window == nullptr) {
    glfwTerminate();
    return -1;
  }

  glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, mouseMoveCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetScrollCallback(window, scrollCallback);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  //glfwSwapInterval(1);

  husky::Log::debug("OpenGL version: %s", glGetString(GL_VERSION));

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC)messageCallback, 0);

  GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
  glCompileShader(vertShader);
  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
  glCompileShader(fragShader);
  GLuint shaderProg = glCreateProgram();
  glAttachShader(shaderProg, vertShader);
  glAttachShader(shaderProg, fragShader);
  glLinkProgram(shaderProg);

  husky::Image image(2, 2, sizeof(husky::Vector4b));
  image.setPixel(0, 0, husky::Vector4b(255, 255, 255, 255));
  image.setPixel(1, 0, husky::Vector4b(128, 128, 128, 255));
  image.setPixel(0, 1, husky::Vector4b(128, 128, 128, 255));
  image.setPixel(1, 1, husky::Vector4b(255, 255, 255, 255));

  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bytes.data());
  //glGenerateMipmap(GL_TEXTURE_2D);

  Material defaultMaterial(shaderProg, tex);
  std::vector<Entity> entities;

  {
    husky::SimpleMesh mesh = husky::Primitive::sphere(1.0);
    mesh.setAllVertexColors({ 0, 255, 0, 255 });

    Entity entity(defaultMaterial, mesh);
    entity.transform = husky::Matrix44d::scale({ 1, 1, 1 });
    entities.emplace_back(entity);
  }

  {
    husky::SimpleMesh mesh = husky::Primitive::cylinder(0.5, 2.0, true);
    mesh.setAllVertexColors({ 255, 0, 255, 255 });

    Entity entity(defaultMaterial, mesh);
    entity.transform = husky::Matrix44d::translate({ 4, 0, 0 });
    entities.emplace_back(entity);
  }

  {
    husky::SimpleMesh mesh = husky::Primitive::box(2.0, 3.0, 1.0);
    mesh.setAllVertexColors({ 255, 0, 0, 255 });

    Entity entity(defaultMaterial, mesh);
    entity.transform = husky::Matrix44d::translate({ -4, 0, 0 });
    entities.emplace_back(entity);
  }

  {
    husky::SimpleMesh mesh = husky::Primitive::torus(8.0, 1.0);
    mesh.setAllVertexColors({ 255, 255, 0, 255 });

    Entity entity(defaultMaterial, mesh);
    entity.transform = husky::Matrix44d::translate({ 0, 0, 0 });
    entities.emplace_back(entity);
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  //glCullFace(GL_FRONT);

  while (!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    frameTime = (time - prevTime);
    prevTime = time;
    //std::cout << frameTime << std::endl;

    handleInput(window);

    glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

    cam.projection = husky::Matrix44d::perspectiveInf(husky::math::deg2rad * 60.0, viewport.aspectRatio(), 0.1, 2.4e-7);
    cam.buildViewMatrix();

    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    glClearColor(0.f, 0.f, .5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (const Entity &entity : entities) {
      entity.draw(cam);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
