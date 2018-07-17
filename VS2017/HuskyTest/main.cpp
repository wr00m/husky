#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Entity.hpp"
#include "UnitTest.hpp"
#include <husky/image/Image.hpp>
#include <husky/math/Intersect.hpp>
#include <husky/mesh/Model.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

static const char *lineVertSrc =
R"(#version 400 core
uniform mat4 modelView;
uniform mat4 projection;
in vec3 vPosition;
in vec4 vColor;
out vec4 vsColor;
void main()
{
  vsColor = vColor;
  gl_Position = projection * (modelView * vec4(vPosition, 1.0));
})";

static const char *lineGeomSrc =
R"(#version 400 core
uniform vec2 viewportSize = vec2(1280, 720); // Pixels
uniform float lineWidth = 2.0; // Pixels
in vec4 vsColor[2];
out vec4 gsColor;
layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;
void main()
{
  vec4 p0 = gl_in[0].gl_Position;
  vec4 p1 = gl_in[1].gl_Position;
  vec3 ndc0 = p0.xyz / p0.w;
  vec3 ndc1 = p1.xyz / p1.w;

  vec2 lineScreenForward = normalize(ndc1.xy - ndc0.xy);
  vec2 lineScreenRight = vec2(-lineScreenForward.y, lineScreenForward.x);
  vec2 lineScreenOffset = (vec2(lineWidth) / viewportSize) * lineScreenRight;

  gl_Position = vec4(p0.xy + lineScreenOffset * p0.w, p0.zw);
  gsColor = vsColor[0];
  EmitVertex();

  gl_Position = vec4(p0.xy - lineScreenOffset * p0.w, p0.zw);
  gsColor = vsColor[0];
  EmitVertex();

  gl_Position = vec4(p1.xy + lineScreenOffset * p1.w, p1.zw);
  gsColor = vsColor[1];
  EmitVertex();

  gl_Position = vec4(p1.xy - lineScreenOffset * p1.w, p1.zw);
  gsColor = vsColor[1];
  EmitVertex();

  EndPrimitive();
})";

static const char *lineFragSrc =
R"(#version 400 core
in vec4 gsColor;
out vec4 fsColor;
void main()
{
  fsColor = gsColor;
}
)";

static const char *defaultVertSrc =
R"(#version 400 core
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

static const char *defaultFragSrc =
R"(#version 400 core
uniform sampler2D tex;
uniform vec3 lightDir = normalize(vec3(2.0, -4.0, 10.0));
uniform vec3 diffuseLightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 ambientLightColor = vec3(0.05, 0.05, 0.05);
in vec3 varNormal;
in vec2 varTexCoord;
in vec4 varColor;
out vec4 fragColor;
void main() {
  //vec3 normal = normalize(varNormal);
  float diffuseLightIntensity = clamp(dot(varNormal, lightDir), 0.0, 1.0);
  vec3 totalLightColor = (diffuseLightIntensity * diffuseLightColor + ambientLightColor);
  vec4 texColor = texture(tex, varTexCoord);
  fragColor = vec4(totalLightColor * varColor.rgb * texColor.rgb, varColor.a * texColor.a);
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
static husky::Vector2d mousePos(0, 0);
static husky::Vector2i windowedPos(0, 0);
static husky::Vector2i windowedSize(1280, 720);
static husky::Viewport viewport;
static double prevTime = 0.0;
static double frameTime = (1.0 / 60.0);
static bool mouseDragRight = false;
static std::vector<std::unique_ptr<Entity>> entities;
static const Entity *selectedEntity = nullptr;
static GLuint fbo = 0;
static husky::Viewport fboViewport;
static bool guiHasFocus = false;

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
  husky::Vector2d newMousePos(x, y);
  husky::Vector2d mouseDelta = newMousePos - mousePos;

  //if (newMousePos.x < 0 || newMousePos.y < 0) {
  //  std::cout << "pos   : " << newMousePos.x << " " << newMousePos.y << std::endl;
  //  std::cout << "delta : " << mouseDelta.x << " " << mouseDelta.y << std::endl;
  //}

  if (mouseDragRight) {
    const double rotSpeed = 0.01;
    cam.attitude = husky::Quaterniond::fromAxisAngle(rotSpeed * -mouseDelta.x, { 0, 0, 1 }) * cam.attitude; // Yaw
    cam.attitude = husky::Quaterniond::fromAxisAngle(rotSpeed * -mouseDelta.y, cam.right()) * cam.attitude; // Pitch
  }

  mousePos = newMousePos;
}

static void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods)
{
  if (guiHasFocus) {
    return;
  }

  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      const Entity *prevSelectedEntity = selectedEntity;
      selectedEntity = nullptr;
      for (const auto &entity : entities) {
        if (entity.get() == prevSelectedEntity) {
          continue;
        }

        husky::Vector2i windowSize;
        glfwGetWindowSize(win, &windowSize.x, &windowSize.y);
        husky::Vector2d windowPos(mousePos.x, windowSize.y - mousePos.y);

        husky::Matrix44d inv = entity->transform.inverted();
        husky::Vector3d rayStart = (inv * husky::Vector4d(cam.position, 1.0)).xyz;
        husky::Vector3d rayDir = inv.get3x3() * viewport.getPickingRayDir(windowPos, cam);
        rayDir = -rayDir; // Reverse Z

        double t0, t1;
        if (husky::Intersect::lineIntersectsBox(rayStart, rayDir, entity->bboxLocal.min, entity->bboxLocal.max, t0, t1) && t0 > 0 && t1 > 0) {
          selectedEntity = entity.get();
          break;
        }
      }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      mouseDragRight = true;
    }
  }
  else if (action == GLFW_RELEASE) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      mouseDragRight = false;
    }
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
  input.x = glfwGetKey(win, GLFW_KEY_D)     - glfwGetKey(win, GLFW_KEY_A);
  input.y = glfwGetKey(win, GLFW_KEY_W)     - glfwGetKey(win, GLFW_KEY_S);
  input.z = glfwGetKey(win, GLFW_KEY_SPACE) - glfwGetKey(win, GLFW_KEY_LEFT_CONTROL);

  const husky::Vector3d camSpeed(20, 20, 20);

  cam.position += cam.right()   * input.x * camSpeed.x * frameTime;
  cam.position += cam.forward() * input.y * camSpeed.y * frameTime;
  cam.position += cam.up()      * input.z * camSpeed.z * frameTime;
}

static void updateViewportAndRebuildFbo()
{
  glfwGetFramebufferSize(window, &viewport.width, &viewport.height);
  fboViewport.set(0, 0, viewport.width, viewport.height);

  GLuint color, depth;
  glGenTextures(1, &color);
  glBindTexture(GL_TEXTURE_2D, color);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_SRGB8_ALPHA8, fboViewport.width, fboViewport.height);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenTextures(1, &depth);
  glBindTexture(GL_TEXTURE_2D, depth);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, fboViewport.width, fboViewport.height);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

  GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
    husky::Log::error("Framebuffer status: %x", fboStatus);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void windowSizeCallback(GLFWwindow *win, int width, int height)
{
  updateViewportAndRebuildFbo();
}

static void initRenderDataGPU(husky::Model &mdl)
{
  for (husky::RenderData &renderData : mdl.meshRenderDatas) {
    glGenBuffers(1, &renderData.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderData.vbo);
    glBufferData(GL_ARRAY_BUFFER, renderData.bytes.size(), renderData.bytes.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &renderData.vao);
    //glBindVertexArray(renderData.vao);
  }
}

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

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  husky::Log::debug("OpenGL version: %s", glGetString(GL_VERSION));

  GLint glMajor, glMinor;
  glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
  glGetIntegerv(GL_MINOR_VERSION, &glMinor);
  if (glMajor < 4 || (glMajor == 4 && glMinor < 5)) {
    husky::Log::error("OpenGL 4.5 or greater required"); // GL 4.5+ required for glClipControl()
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO(); //(void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ImGui_ImplGlfw_InitForOpenGL(window, false);
  ImGui_ImplOpenGL3_Init(nullptr);
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  glfwSetWindowSizeCallback(window, windowSizeCallback);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, mouseMoveCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetScrollCallback(window, scrollCallback);

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC)messageCallback, 0);

  updateViewportAndRebuildFbo();

  GLuint defaultVertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(defaultVertShader, 1, &defaultVertSrc, NULL);
  glCompileShader(defaultVertShader);
  GLuint defaultFragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(defaultFragShader, 1, &defaultFragSrc, NULL);
  glCompileShader(defaultFragShader);
  GLuint defaultShaderProg = glCreateProgram();
  glAttachShader(defaultShaderProg, defaultVertShader);
  glAttachShader(defaultShaderProg, defaultFragShader);
  glLinkProgram(defaultShaderProg);

  GLuint lineVertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(lineVertShader, 1, &lineVertSrc, NULL);
  glCompileShader(lineVertShader);
  GLuint lineGeomShader = glCreateShader(GL_GEOMETRY_SHADER);
  glShaderSource(lineGeomShader, 1, &lineGeomSrc, NULL);
  glCompileShader(lineGeomShader);
  GLuint lineFragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(lineFragShader, 1, &lineFragSrc, NULL);
  glCompileShader(lineFragShader);
  GLuint lineShaderProg = glCreateProgram();
  glAttachShader(lineShaderProg, lineVertShader);
  glAttachShader(lineShaderProg, lineGeomShader);
  glAttachShader(lineShaderProg, lineFragShader);
  glLinkProgram(lineShaderProg);

  husky::Image image(2, 2, sizeof(husky::Vector4b));
  image.setPixel(0, 0, husky::Vector4b(255, 255, 255, 255));
  image.setPixel(1, 0, husky::Vector4b(128, 128, 128, 255));
  image.setPixel(0, 1, husky::Vector4b(128, 128, 128, 255));
  image.setPixel(1, 1, husky::Vector4b(255, 255, 255, 255));

  GLuint textureHandle;
  glGenTextures(1, &textureHandle);
  glBindTexture(GL_TEXTURE_2D, textureHandle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bytes.data());
  //glGenerateMipmap(GL_TEXTURE_2D);

  Shader defaultShader(defaultShaderProg);
  defaultShader.textureHandle = textureHandle;

  Shader lineShader(lineShaderProg);

  {
    husky::SimpleMesh mesh = husky::Primitive::sphere(1.0);
    mesh.setAllVertexColors({ 0, 255, 0, 255 });

    auto entity = std::make_unique<Entity>("Sphere", defaultShader, lineShader, std::move(mesh));
    entity->transform = husky::Matrix44d::scale({ 1, 1, 1 });
    //entities.emplace_back(std::move(entity));
  }

  {
    husky::SimpleMesh mesh = husky::Primitive::cylinder(0.5, 2.0, true);
    mesh.setAllVertexColors({ 255, 0, 255, 255 });

    auto entity = std::make_unique<Entity>("Cylinder", defaultShader, lineShader, std::move(mesh));
    entity->transform = husky::Matrix44d::translate({ 4, 0, 0 });
    entities.emplace_back(std::move(entity));
  }

  {
    husky::SimpleMesh mesh = husky::Primitive::box(2.0, 3.0, 1.0);
    mesh.setAllVertexColors({ 255, 0, 0, 255 });

    auto entity = std::make_unique<Entity>("Box", defaultShader, lineShader, std::move(mesh));
    entity->transform = husky::Matrix44d::translate({ -4, 0, 0 });
    entities.emplace_back(std::move(entity));
  }

  {
    husky::SimpleMesh mesh = husky::Primitive::torus(8.0, 1.0);
    mesh.setAllVertexColors({ 255, 255, 0, 255 });

    auto entity = std::make_unique<Entity>("Torus", defaultShader, lineShader, std::move(mesh));
    entity->transform = husky::Matrix44d::translate({ 0, 0, 0 });
    entities.emplace_back(std::move(entity));
  }

  {
    //husky::Model mdl = husky::Model::load("C:/Users/chris/Stash/Blender/Explora/character.fbx");
    husky::Model mdl = husky::Model::load("C:/Users/chris/Stash/Blender/Explora/character.blend");
    //husky::Model mdl = husky::Model::load("C:/Users/chris/Stash/Git/boynbot/Assets/Models/Bot.fbx");
    //husky::Model mdl = husky::Model::load("C:/Users/chris/Stash/Git/boynbot/Assets/Models/Boy.fbx");
    //mesh.setAllVertexColors({ 255, 255, 0, 255 });

    auto entity = std::make_unique<Entity>("TestModel", defaultShader, lineShader, std::move(mdl));
    entity->transform = husky::Matrix44d::translate({ 0, 0, 0 });
    entities.emplace_back(std::move(entity));
  }

  for (auto &entity : entities) {
    initRenderDataGPU(entity->model); // TODO: Remove
    initRenderDataGPU(entity->bboxModel); // TODO: Remove
  }

  glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE); // Change clip space Z range from [-1,1] to [0,1]
  glEnable(GL_DEPTH_TEST);
  //glDisable(GL_CULL_FACE);
  //glCullFace(GL_FRONT);

  while (!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    frameTime = (time - prevTime);
    prevTime = time;
    //std::cout << frameTime << std::endl;

    if (!guiHasFocus) {
      handleInput(window);
    }

    cam.projection = husky::Matrix44d::perspectiveInfRevZ(husky::Math::deg2rad * 60.0, fboViewport.aspectRatio(), 0.1); // , 2.4e-7);
    cam.buildViewMatrix();

    { // Render scene to FBO
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);

      glViewport(fboViewport.x, fboViewport.y, fboViewport.width, fboViewport.height);
      glClearColor(0.f, 0.f, .5f, 1.f);
      glClearDepth(0.0f); // Reverse Z
      glDepthFunc(GL_GREATER); // Reverse Z
      glEnable(GL_DEPTH_CLAMP); // http://www.terathon.com/gdc07_lengyel.pdf
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      for (const auto &entity : entities) {
        bool selected = (entity.get() == selectedEntity);
        entity->draw(viewport, cam, selected);
      }

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    { // Blit FBO to screen
      glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      glBlitFramebuffer(fboViewport.x, fboViewport.y, fboViewport.x + fboViewport.width, fboViewport.y + fboViewport.height,
                           viewport.x,    viewport.y,    viewport.x +    viewport.width,    viewport.y +    viewport.height,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST); // GL_LINEAR
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    { // Render GUI
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      ImGui::Begin("Debug");

      guiHasFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);

      ImGui::Text("Test...");
      ImGui::End();

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
