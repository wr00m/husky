#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <husky/render/Camera.hpp>
#include <husky/render/Entity.hpp>
#include <husky/image/Image.hpp>
#include <husky/math/Intersect.hpp>
#include <husky/mesh/Model.hpp>
#include <husky/math/EulerAngles.hpp>
#include <husky/render/Texture.hpp>
#include <husky/util/SharedResource.hpp>
#include "UnitTest.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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
static std::vector<std::unique_ptr<husky::Model>> models;
static std::vector<std::unique_ptr<husky::Entity>> entities;
static int iSelectedEntity = -1;
static GLuint fbo = 0;
static husky::Viewport fboViewport;
static ImGuiIO *io = nullptr;

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
    if (key == GLFW_KEY_F11) { // Fullscreen
      toggleFullscreen(win);
    }
    else if (key == GLFW_KEY_ESCAPE) { // Exit
      glfwSetWindowShouldClose(win, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_PAGE_UP) { // Select previous entity
      if (!entities.empty()) {
        iSelectedEntity = (std::max(iSelectedEntity, 0) + (int)entities.size() - 1) % entities.size();
      }
    }
    else if (key == GLFW_KEY_PAGE_DOWN) { // Select next entity
      if (!entities.empty()) {
        iSelectedEntity = (iSelectedEntity + 1) % entities.size();
      }
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
    cam.rot = husky::Quaterniond::fromAxisAngle(rotSpeed * -mouseDelta.x, { 0, 0, 1 }) * cam.rot; // Yaw
    cam.rot = husky::Quaterniond::fromAxisAngle(rotSpeed * -mouseDelta.y, cam.right()) * cam.rot; // Pitch
  }

  mousePos = newMousePos;
}

static void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods)
{
  if (io->WantCaptureMouse) {
    return;
  }

  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      std::multimap<double, int> clickedEntities; // Sorted by key (tMean)

      for (int iEntity = 0; iEntity < (int)entities.size(); iEntity++) {
        const auto &entity = entities[iEntity];

        husky::Vector2i windowSize;
        glfwGetWindowSize(win, &windowSize.x, &windowSize.y);
        husky::Vector2d windowPos(mousePos.x, windowSize.y - mousePos.y);

        // TODO: Each entity should provide its bounding box in world coordinates (better performance than inverting the picking ray for each entity)
        husky::Matrix44d inv = entity->transform.inverted();
        husky::Vector3d rayStart = (inv * husky::Vector4d(cam.pos, 1.0)).xyz;
        husky::Vector3d rayDir = (inv * husky::Vector4d(viewport.getPickingRayDir(windowPos, cam), 0)).xyz;

        double t0, t1;
        if (husky::Intersect::lineIntersectsBox(rayStart, rayDir, entity->bboxLocal.min, entity->bboxLocal.max, t0, t1) && t0 > 0 && t1 > 0) {
          double tMean = (t0 + t1) * 0.5; // Picking priority feels more intuitive with tMean than t0
          clickedEntities.insert({ tMean, iEntity });
        }
      }

      if (clickedEntities.empty()) { // Nothing clicked => Deselect
        iSelectedEntity = -1;
      }
      else if (clickedEntities.size() == 1) { // One entity clicked => Select the entity
        iSelectedEntity = clickedEntities.begin()->second;
      }
      else { // Multiple entities clicked => Select first unselected entity
        for (const auto &pair : clickedEntities) {
          if (pair.second != iSelectedEntity) {
            iSelectedEntity = pair.second;
            break;
          }
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
  cam.pos += cam.forward() * zoomSpeed * deltaY;
}

static void handleKeyInput(GLFWwindow *win)
{
  husky::Vector3d input;
  input.x = glfwGetKey(win, GLFW_KEY_D)     - glfwGetKey(win, GLFW_KEY_A);
  input.y = glfwGetKey(win, GLFW_KEY_W)     - glfwGetKey(win, GLFW_KEY_S);
  input.z = glfwGetKey(win, GLFW_KEY_SPACE) - glfwGetKey(win, GLFW_KEY_LEFT_CONTROL);

  const husky::Vector3d camSpeed(20, 20, 20);

  cam.pos += cam.right()   * input.x * camSpeed.x * frameTime;
  cam.pos += cam.forward() * input.y * camSpeed.y * frameTime;
  cam.pos += cam.up()      * input.z * camSpeed.z * frameTime;
}

static void updateViewportAndRebuildFbo()
{
  glfwGetFramebufferSize(window, &viewport.width, &viewport.height);
  
  cam.aspectRatio = viewport.aspectRatio();
  cam.buildProjMatrix();

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
  io = &ImGui::GetIO(); //(void)io;
  //io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  //io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
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

  const husky::Shader defaultShader = husky::Shader::getDefaultShader(false);
  const husky::Shader defaultShaderBone = husky::Shader::getDefaultShader(true);
  const husky::Shader lineShader = husky::Shader::getDefaultLineShader();

#if 1
  husky::Image image(2, 2, husky::ImageFormat::RGBA8);
  image.setPixel(0, 0, husky::Vector4b(255, 255, 255, 255));
  image.setPixel(1, 0, husky::Vector4b(128, 128, 128, 255));
  image.setPixel(0, 1, husky::Vector4b(128, 128, 128, 255));
  image.setPixel(1, 1, husky::Vector4b(255, 255, 255, 255));
  //image.setPixel(0, 0, husky::Vector4b(255, 0,   0,   255));
  //image.setPixel(1, 0, husky::Vector4b(0,   255, 0,   255));
  //image.setPixel(0, 1, husky::Vector4b(0,   0,   255, 255));
  //image.setPixel(1, 1, husky::Vector4b(255, 255, 0,   255));
#else
  husky::Image image = husky::Image::load("C:/tmp/test.jpg");
  image.save("C:/tmp/imgout/test.png");
#endif

  GLuint textureHandle = husky::Texture::uploadTexture(image);

  {
    models.emplace_back(std::make_unique<husky::Model>(husky::Mesh::sphere(1.0), husky::Material({ 0, 1, 0 }, textureHandle)));
    entities.emplace_back(std::make_unique<husky::Entity>("Sphere", &defaultShader, &lineShader, models.back().get()));
    entities.back()->transform = husky::Matrix44d::translate({ 3, 3, 0 });
  }

  {
    models.emplace_back(std::make_unique<husky::Model>(husky::Mesh::cylinder(0.5, 0.3, 2.0, true, false, 8, 1), husky::Material({ 1, 0, 1 }, textureHandle)));
    entities.emplace_back(std::make_unique<husky::Entity>("Cylinder", &defaultShader, &lineShader, models.back().get()));
    entities.back()->transform = husky::Matrix44d::translate({ 4, -2, 0 });
  }

  {
    models.emplace_back(std::make_unique<husky::Model>(husky::Mesh::cone(0.5, 1.0, true, 8), husky::Material({ 1, 0, 1 }, textureHandle)));
    entities.emplace_back(std::make_unique<husky::Entity>("Cone", &defaultShader, &lineShader, models.back().get()));
    entities.back()->transform = husky::Matrix44d::translate({ 4, -2, 2 });
  }

  {
    models.emplace_back(std::make_unique<husky::Model>(husky::Mesh::box(2.0, 3.0, 1.0), husky::Material({ 1, 0, 0 }, textureHandle)));
    entities.emplace_back(std::make_unique<husky::Entity>("Box", &defaultShader, &lineShader, models.back().get()));
    entities.back()->transform = husky::Matrix44d::translate({ -4, 0, 0 }) * husky::Matrix44d::rotate(husky::Math::pi2, { 0, 0, 1 });
  }

  {
    models.emplace_back(std::make_unique<husky::Model>(husky::Mesh::torus(8.0, 1.0), husky::Material({ 1, 1, 0 }, textureHandle)));
    entities.emplace_back(std::make_unique<husky::Entity>("Torus", &defaultShader, &lineShader, models.back().get()));
    entities.back()->transform = husky::Matrix44d::translate({ 0, 0, 0 });
  }

  {
    //husky::Model mdl = husky::Model::load("C:/Users/chris/Stash/Blender/Explora/character.fbx");
    //husky::Model mdl = husky::Model::load("C:/Users/chris/Stash/Blender/Explora/character.blend");
    //husky::Model mdl = husky::Model::load("C:/Users/chris/Stash/Git/boynbot/Assets/Models/Bot.fbx");
    //husky::Model mdl = husky::Model::load("C:/Users/chris/Stash/Blender/BoynBot/Bot/Bot.blend");
    //husky::Model mdl = husky::Model::load("C:/Users/chris/Stash/Git/boynbot/Assets/Models/Boy.fbx");
    //husky::Model mdl = husky::Model::load("C:/tmp/Rigged_Hand_fbx/Rigged Hand.fbx");
    husky::Model mdl = husky::Model::load("C:/tmp/Box/Box.blend");

    models.emplace_back(std::make_unique<husky::Model>(std::move(mdl)));
    entities.emplace_back(std::make_unique<husky::Entity>("TestModel", &defaultShader, &lineShader, models.back().get()));
    entities.back()->transform = husky::Matrix44d::rotate(husky::Math::pi2, { 1, 0, 0 }); // * husky::Matrix44d::scale({ 0.01, 0.01, 0.01 });
  }

  for (auto &entity : entities) {
    initRenderDataGPU(*entity->modelInstance.model); // TODO: Remove
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
    double fps = (frameTime != 0 ? (1.0 / frameTime) : 0.0);

    if (!io->WantCaptureKeyboard) {
      handleKeyInput(window);
    }

    cam.buildViewMatrix();

    std::vector<int> viewEntities;

    { // Render scene to FBO
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);

      glViewport(fboViewport.x, fboViewport.y, fboViewport.width, fboViewport.height);
      glClearColor(0.f, 0.f, .5f, 1.f);
      glClearDepth(0.0f); // Reverse Z
      glDepthFunc(GL_GREATER); // Reverse Z
      glEnable(GL_DEPTH_CLAMP); // http://www.terathon.com/gdc07_lengyel.pdf
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      const husky::Frustum frustum = cam.frustum();

      for (int iEntity = 0; iEntity < (int)entities.size(); iEntity++) {
        const auto &entity = entities[iEntity];

        bool isSelected = (iEntity == iSelectedEntity);
        entity->draw(viewport, cam, isSelected);

        // TODO: Don't test frustum intersection with *local* bounding box!
        if ((bool)frustum.touches(entity->bboxLocal.min, entity->bboxLocal.max)) {
          viewEntities.emplace_back(iEntity);
        }
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

      std::string entitiesDebugText;
      {
        std::ostringstream oss;
        oss << "Entities in view: " << viewEntities.size();
        for (const auto iViewEntity : viewEntities) {
          oss << "\n  " << (iViewEntity == iSelectedEntity ? "*" : "") << entities[iViewEntity]->name;
        }
        entitiesDebugText = oss.str();
      }

      ImGui::Text("fps: %d", (int)std::round(fps));
      ImGui::Text("cam.pos:\n  %f\n  %f\n  %f", cam.pos.x, cam.pos.y, cam.pos.z);

      int projMode = (int)cam.projMode;
      if (ImGui::Combo("cam.projMode", &projMode, "ORTHO\0PERSP\0PERSP_FARINF\0PERSP_FARINF_REVZ")) {
        cam.projMode = (husky::ProjectionMode)projMode;
        cam.buildProjMatrix();
      }

      float fov = (float)cam.perspVerticalFovRad;
      if (ImGui::SliderAngle("fov", &fov, 1.f, 179.f)) {
        cam.perspVerticalFovRad = fov;
        cam.buildProjMatrix();
      }

      //ImGui::Text("cam.fov: %f", cam.projection.fov() * husky::Math::rad2deg);
      ImGui::Text(entitiesDebugText.c_str());

      if (iSelectedEntity != -1) {
        const auto &selectedEntity = entities[iSelectedEntity];
        
        if (ImGui::Button("Zoom to selected")) {
          // TODO
          //husky::Vector3d boundingSphereCenterPt = selectedEntity->bboxLocal.center();
          //double boundingSphereRadius = (selectedEntity->bboxLocal.max - selectedEntity->bboxLocal.min).length() * 0.5;
        }
        
        husky::Vector3d scale, trans;
        husky::Matrix33d rot;
        selectedEntity->transform.decompose(scale, rot, trans);

        husky::EulerAnglesf eulerAngles = (husky::EulerAnglesf)husky::EulerAnglesd(husky::RotationOrder::ZXY, rot);
        float yaw   = (float)eulerAngles.yaw;
        float pitch = (float)eulerAngles.pitch;
        float roll  = (float)eulerAngles.roll;

        bool rotated = false;
        rotated |= ImGui::SliderAngle("Yaw",   &yaw,   -180.f, 180.f);
        rotated |= ImGui::SliderAngle("Pitch", &pitch, -180.f, 180.f);
        rotated |= ImGui::SliderAngle("Roll",  &roll,  -180.f, 180.f);

        if (rotated) {
          eulerAngles.angles.set(yaw, pitch, roll);
          selectedEntity->transform = husky::Matrix44d::compose(scale, ((husky::EulerAnglesd)eulerAngles).toMatrix(), trans);
        }
      }

      ImGui::Image(((std::uint8_t*)nullptr) + textureHandle, { 100, 100 });

      ImGui::End();

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  husky::SharedResource::releaseAll();

  return 0;
}
