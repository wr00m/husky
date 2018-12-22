#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <husky/render/Camera.hpp>
#include <husky/render/Component.hpp>
#include <husky/render/Entity.hpp>
#include <husky/render/Billboard.hpp>
#include <husky/geo/Shapefile.hpp>
#include <husky/image/Image.hpp>
#include <husky/mesh/Model.hpp>
#include <husky/mesh/Tessellator.hpp>
#include <husky/math/Intersect.hpp>
#include <husky/math/EulerAngles.hpp>
#include <husky/math/Random.hpp>
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

static void setSelectedEntity(int i)
{
  if (i == iSelectedEntity) {
    return;
  }

  // Deselect previous
  if (iSelectedEntity != -1) {
    entities[iSelectedEntity]->removeComponent<husky::DebugDrawComponent>();
    iSelectedEntity = -1;
  }

  // Select
  if (i != -1) {
    entities[i]->addComponent<husky::DebugDrawComponent>();
    iSelectedEntity = i;
  }
}

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
        setSelectedEntity((std::max(iSelectedEntity, 0) + (int)entities.size() - 1) % entities.size());
      }
    }
    else if (key == GLFW_KEY_PAGE_DOWN) { // Select next entity
      if (!entities.empty()) {
        setSelectedEntity((iSelectedEntity + 1) % entities.size());
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
      husky::Vector2i windowSize;
      glfwGetWindowSize(win, &windowSize.x, &windowSize.y);

      const husky::Vector2d windowPos(mousePos.x, windowSize.y - mousePos.y);
      const husky::Ray rayWorld = viewport.getPickingRay(windowPos, cam);

      std::multimap<double, int> clickedEntities; // Sorted by key (tMean)

      for (int iEntity = 0; iEntity < (int)entities.size(); iEntity++) {
        const auto &entity = entities[iEntity];

        const husky::Matrix44d inv = entity->getTransform().inverted(); // TODO: Use pre-inverted transform, or get bounds in world coordinates
        const husky::Ray ray = (inv * rayWorld);

        double t0, t1;
        if (husky::Intersect::lineIntersectsBox(ray.startPos, ray.dir, entity->bboxLocal.min, entity->bboxLocal.max, t0, t1) && t0 > 0 && t1 > 0) {
          double tMean = (t0 + t1) * 0.5; // Picking priority feels more intuitive with tMean than t0
          clickedEntities.insert({ tMean, iEntity });
        }
      }

      if (clickedEntities.empty()) { // Nothing clicked => Deselect
        setSelectedEntity(-1);
      }
      else if (clickedEntities.size() == 1) { // One entity clicked => Select the entity
        setSelectedEntity(clickedEntities.begin()->second);
      }
      else { // Multiple entities clicked => Select first unselected entity
        for (const auto &pair : clickedEntities) {
          if (pair.second != iSelectedEntity) {
            setSelectedEntity(pair.second);
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

  const double camSpeedMultiplier = (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) ? 3.0 : 1.0);
  const husky::Vector3d camSpeed(20.0 * camSpeedMultiplier);

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

  static const husky::Shader defaultShader = husky::Shader::getDefaultShader(true, false);
  static const husky::Shader defaultShaderBones = husky::Shader::getDefaultShader(true, true);
  static const husky::Shader lineShader = husky::Shader::getLineShader();
  static const husky::Shader billboardShader = husky::Billboard::getBillboardShader(husky::BillboardMode::SPHERICAL);

  husky::Image image(2, 2, husky::ImageFormat::RGBA8);
  image.setPixel(0, 0, husky::Vector4b(255, 255, 255, 255));
  image.setPixel(1, 0, husky::Vector4b(128, 128, 128, 255));
  image.setPixel(0, 1, husky::Vector4b(128, 128, 128, 255));
  image.setPixel(1, 1, husky::Vector4b(255, 255, 255, 255));
  const husky::Texture tex = husky::Texture(image, husky::TexWrap::REPEAT, husky::TexFilter::NEAREST, husky::TexMipmaps::NONE);

  {
    models.emplace_back(std::make_unique<husky::Model>(husky::Mesh::sphere(1.0), husky::Material({ 0, 1, 0 }, tex)));
    entities.emplace_back(std::make_unique<husky::Entity>("Sphere", &defaultShader, models.back().get()));
    entities.back()->setTransform(husky::Matrix44d::translate({ 3, 3, 0 }));
  }

  {
    models.emplace_back(std::make_unique<husky::Model>(husky::Mesh::cylinder(0.5, 0.3, 2.0, true, false, 8, 1), husky::Material({ 1, 0, 1 }, tex)));
    entities.emplace_back(std::make_unique<husky::Entity>("Cylinder", &defaultShader, models.back().get()));
    entities.back()->setTransform(husky::Matrix44d::translate({ 4, -2, 0 }));
  }

  {
    models.emplace_back(std::make_unique<husky::Model>(husky::Mesh::cone(0.5, 1.0, true, 8), husky::Material({ 1, 0, 1 }, tex)));
    entities.emplace_back(std::make_unique<husky::Entity>("Cone", &defaultShader, models.back().get()));
    entities.back()->setTransform(husky::Matrix44d::translate({ 4, -2, 2 }));
  }

  {
    models.emplace_back(std::make_unique<husky::Model>(husky::Mesh::box(2.0, 3.0, 1.0), husky::Material({ 1, 0, 0 }, tex)));
    entities.emplace_back(std::make_unique<husky::Entity>("Box", &defaultShader, models.back().get()));
    entities.back()->setTransform(husky::Matrix44d::translate({ -20, 0, 0 }) * husky::Matrix44d::rotate(husky::Math::pi2, { 0, 0, 1 }));
  }

  {
    models.emplace_back(std::make_unique<husky::Model>(husky::Mesh::torus(8.0, 1.0), husky::Material({ 1, 1, 0 }, tex)));
    entities.emplace_back(std::make_unique<husky::Entity>("Torus", &defaultShader, models.back().get()));
    entities.back()->setTransform(husky::Matrix44d::translate({ 0, 0, 0 }));
  }

  {
    models.emplace_back(std::make_unique<husky::Model>(husky::Model::load("C:/Users/chris/Stash/Blender/BoynBot/Bot/Bot.fbx")));
    entities.emplace_back(std::make_unique<husky::Entity>("Bot", &defaultShaderBones, models.back().get()));
    entities.back()->setTransform(husky::Matrix44d::compose({ 1, 1, 1 }, husky::Matrix33d::rotate(husky::Math::pi2, { 1, 0, 0 }), { -3, 0, 0 }));
  }

  {
    //husky::Model mdl = husky::Model::load("C:/Users/chris/Stash/Blender/Explora/character.fbx");
    husky::Model mdl = husky::Model::load("C:/Users/chris/Stash/Blender/BoynBot/Boy/Boy_FBX2013.fbx");
    models.emplace_back(std::make_unique<husky::Model>(std::move(mdl)));
    entities.emplace_back(std::make_unique<husky::Entity>("Boy", &defaultShaderBones, models.back().get()));
    entities.back()->modelInstance.mtxTransform = husky::Matrix44d::rotate(husky::Math::pi2, { 1, 0, 0 }) * husky::Matrix44d::translate(-entities.back()->modelInstance.model->bboxLocal.center());
    entities.back()->setTransform(husky::Matrix44d::compose({ 1, 1, 1 }, husky::Matrix33d::identity(), { 1, 0, 0 }));
    //entities.back()->addComponent<husky::DebugDrawComponent>();
  }

  //{
  //  husky::Model mdl = husky::Model::load("C:/tmp/Models/fir1_3ds/firtree1.3ds");
  //  models.emplace_back(std::make_unique<husky::Model>(std::move(mdl)));
  //  entities.emplace_back(std::make_unique<husky::Entity>("Tree", &defaultShaderBones, models.back().get()));
  //  entities.back()->modelInstance.mtxTransform = husky::Matrix44d::rotate(husky::Math::pi2, { 1, 0, 0 }) * husky::Matrix44d::translate(-entities.back()->modelInstance.model->bboxLocal.center());
  //  entities.back()->setTransform(husky::Matrix44d::compose({ 1, 1, 1 }, husky::Matrix33d::identity(), { -5, -5, 0 }));
  //}

  {
    husky::FeatureTable featureTable = husky::Shapefile::load("F:/Geodata/World_Countries/World_Countries.shp");
    husky::Mesh mesh;
    for (const husky::Feature &feature : featureTable._features) {
      if (feature._parts.empty()) {
        continue;
      }
      int iVert0 = mesh.numVerts();
      for (const auto &point : feature._points) {
        mesh.addVert(point.xyz);
      }
      for (int iPart = 0; iPart < (int)feature._parts.size(); iPart++) {
        int iVertBeginIncl = feature._parts[iPart];
        int iVertEndExcl = (iPart == feature._parts.size() - 1 ? (int)feature._points.size() : feature._parts[iPart + 1]);
        for (int iVert = iVertBeginIncl + 1; iVert < iVertEndExcl; iVert++) {
          mesh.addLine(iVert0 + iVert - 1, iVert0 + iVert);
        }
      }
    }
    models.emplace_back(std::make_unique<husky::Model>(husky::Model(std::move(mesh), {})));
    entities.emplace_back(std::make_unique<husky::Entity>("Countries", &lineShader, models.back().get()));
    entities.back()->setTransform(husky::Matrix44d::compose({ 1, 1, 1 }, husky::Matrix33d::identity(), { 0, 0, 0 }));
  }

  {
    //const husky::Texture texTree("C:/tmp/Billboard/tree.png", husky::TexWrap::REPEAT, husky::TexFilter::LINEAR, husky::TexMipmaps::STANDARD);
    const husky::MultidirTexture texBillboard = husky::Billboard::getMultidirectionalBillboardTexture(*entities[6].get(), 8192, 8192, 64, 63);
    //texBillboard.tex.downloadImageData().save("C:/tmp/hejhopp.png");

    husky::Random random;

    husky::Mesh billboardPointsMesh;
    for (int ix = 0; ix < 100; ix++) {
      for (int iy = 0; iy < 100; iy++) {
        int iVert = billboardPointsMesh.addVert(husky::Vector3d(random.getDouble(0, 250), random.getDouble(0, 250), 0));
        billboardPointsMesh.setTexCoord(iVert, husky::Vector2d(random.getDouble() * 0.5 + 1.0));
        billboardPointsMesh.setColor(iVert, husky::Vector4b(random.getInt(100, 255), random.getInt(100, 255), random.getInt(50, 200), 255));
      }
    }

    husky::Material mtl({ 1, 0.5, 0 }, texBillboard.tex);
    husky::Model mdl(std::move(billboardPointsMesh), mtl);
    models.emplace_back(std::make_unique<husky::Model>(std::move(mdl)));
    entities.emplace_back(std::make_unique<husky::Entity>("Billboards", &billboardShader, models.back().get()));
    //entities.back()->setTransform(husky::Matrix44d::identity());
  }

  cam.lookAt({ 0, 0, 0 }, { 0, 0, 1 });

  while (!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    frameTime = (time - prevTime);
    prevTime = time;
    double fps = (frameTime != 0 ? (1.0 / frameTime) : 0.0);

    for (auto &entity : entities) {
      entity->update(frameTime);
    }

    if (!io->WantCaptureKeyboard) {
      handleKeyInput(window);
    }

    cam.buildViewMatrix();

    std::vector<int> viewEntities;

    { // Render scene to FBO
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);

      glViewport(fboViewport.x, fboViewport.y, fboViewport.width, fboViewport.height);
      glClearColor(0.f, 0.f, .5f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      if (cam.isRevZ()) {
        glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE); // Change clip space Z range from [-1,1] to [0,1]
        glClearDepth(0.f);
        glDepthFunc(GL_GREATER);
        glEnable(GL_DEPTH_CLAMP); // http://www.terathon.com/gdc07_lengyel.pdf
      }
      else {
        glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE); // Default clip space
        glClearDepth(1.f);
        glDepthFunc(GL_LESS);
        glDisable(GL_DEPTH_CLAMP);
      }

      glEnable(GL_DEPTH_TEST);

      const husky::Frustum frustum = cam.frustum();

      for (int iEntity = 0; iEntity < (int)entities.size(); iEntity++) {
        const auto &entity = entities[iEntity];
        entity->draw(viewport, cam);

        if ((bool)frustum.touches(entity->bboxLocal, &entity->getTransform())) {
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
      if (ImGui::Combo("cam.projMode", &projMode, "ORTHO\0ORTHO_REVZ\0PERSP\0PERSP_FARINF\0PERSP_FARINF_REVZ")) {
        cam.projMode = (husky::ProjectionMode)projMode;
        cam.buildProjMatrix();
      }

      if (cam.isOrtho()) {
        float orthoHeight = (float)cam.orthoHeight;
        if (ImGui::SliderFloat("orthoHeight", &orthoHeight, 0.1f, 100.0f)) {
          cam.orthoHeight = orthoHeight;
          cam.buildProjMatrix();
        }
      }
      else {
        float fov = (float)cam.vfovRad;
        if (ImGui::SliderAngle("fov", &fov, 1.f, 179.f)) {
          cam.vfovRad = fov;
          cam.buildProjMatrix();
        }
      }

      ImGui::Text(entitiesDebugText.c_str());

      if (iSelectedEntity != -1) {
        const auto &selectedEntity = entities[iSelectedEntity];
        
        if (ImGui::Button("Zoom to selected")) {
          husky::Matrix44d entityTransform = selectedEntity->getTransform();
          double scale = entityTransform.col[0].length(); // Assume uniform scaling

          husky::Sphere bsphereWorld = selectedEntity->bsphereLocal;
          bsphereWorld.center = (entityTransform * husky::Vector4d(bsphereWorld.center, 1)).xyz;
          bsphereWorld.radius *= scale;

          double fovRad = (cam.aspectRatio > 1.0 ? cam.vfovRad : cam.hfovRad());
          double camDistToEntity = bsphereWorld.radius / std::tan(fovRad * 0.5);
          cam.pos = (bsphereWorld.center - cam.forward() * camDistToEntity);
          cam.buildViewMatrix();
        }
        
        if (ImGui::Button("Look at selected")) {
          cam.lookAt(selectedEntity->getTransform()[3].xyz, { 0, 0, 1 });
          cam.buildViewMatrix();
        }

        {
          std::vector<const char*> animNames;
          animNames.emplace_back("<None>");
          for (const auto &anim : selectedEntity->modelInstance.model->animations) {
            animNames.emplace_back(anim.name.c_str());
          }

          int iAnim = (selectedEntity->modelInstance.animationIndex + 1);
          if (ImGui::Combo("Animation", &iAnim, animNames.data(), (int)animNames.size())) {
            selectedEntity->modelInstance.setAnimationIndex(iAnim - 1);
          }

          if (const husky::Animation *anim = selectedEntity->modelInstance.getActiveAnimation()) {
            float animFactor = (float)(anim != nullptr ? (anim->getTicks(selectedEntity->modelInstance.animationTime) / anim->durationTicks) : 0);
            ImGui::ProgressBar(animFactor);
          }
        }

        husky::Vector3d scale, trans;
        husky::Matrix33d rot;
        selectedEntity->getTransform().decompose(scale, rot, trans);

        husky::EulerAnglesf eulerAngles = (husky::EulerAnglesf)husky::EulerAnglesd(husky::RotationOrder::ZXY, rot);

        bool rotated = false;
        rotated |= ImGui::SliderAngle("Yaw",   &eulerAngles.yaw,   -180.f, 180.f);
        rotated |= ImGui::SliderAngle("Pitch", &eulerAngles.pitch, -180.f, 180.f);
        rotated |= ImGui::SliderAngle("Roll",  &eulerAngles.roll,  -180.f, 180.f);

        if (rotated) {
          selectedEntity->setTransform(husky::Matrix44d::compose(scale, ((husky::EulerAnglesd)eulerAngles).toMatrix(), trans));
        }
      }

      ImGui::Image(((std::uint8_t*)nullptr) + tex.handle, { 100, 100 });

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
