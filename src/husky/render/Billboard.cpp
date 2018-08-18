#include <husky/render/Billboard.hpp>
#include <husky/Log.hpp>
#include <glad/glad.h>

namespace husky {

Shader Billboard::getBillboardShader(BillboardMode mode)
{
  static const char *billboardVertSrc =
R"(//#version 400 core
uniform mat4 mtxModelView;
in vec3 vertPosition;
in vec2 vertTexCoord; // TODO: Don't abuse texture coordinate as scale
in vec4 vertColor;
out vec2 vsScale;
out vec4 vsColor;
void main()
{
  vsColor = vertColor;
  vsScale = vertTexCoord;
  //mat4 MV = mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(mtxModelView[3].xyz, 1));
  gl_Position = mtxModelView * vec4(vertPosition, 1.0);
})";

  static const char *billboardGeomSrc =
R"(//#version 400 core
uniform mat4 mtxModelView;
uniform mat4 mtxProjection;
#if defined(BILLBOARD_FIXED_PX)
uniform vec2 viewportSize = vec2(1280, 720); // Pixels
uniform vec2 billboardSizePx = vec2(64, 64); // Pixels
#else
#if defined(BILLBOARD_VIEWPLANE_CYLINDRICAL) || defined(BILLBOARD_CYLINDRICAL)
uniform vec3 cylindricalUpDir = vec3(0, 0, 1);
#endif
uniform vec2 billboardSize = vec2(1, 1); // World units
#endif
uniform vec2 texCoordScale = vec2(1.0, -1.0); // TODO: This won't work with repeating textures
in vec2 vsScale[1];
in vec4 vsColor[1];
out vec2 gsTexCoord;
out vec4 gsColor;
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

void emitBillboardVert(const vec2 offset)
{
#if defined(BILLBOARD_VIEWPLANE_SPHERICAL)
  gl_Position = gl_in[0].gl_Position;
  gl_Position.xy += (offset * billboardSize * vsScale[0]);
  gl_Position = (mtxProjection * gl_Position);
#elif defined(BILLBOARD_VIEWPLANE_CYLINDRICAL)
  vec3 up = (mtxModelView * vec4(cylindricalUpDir, 0.0)).xyz;
  gl_Position = gl_in[0].gl_Position;
  gl_Position.x += (offset.x * billboardSize.x * vsScale[0].x);
  gl_Position.xyz += (up * offset.y * billboardSize.y * vsScale[0].y);
  gl_Position = (mtxProjection * gl_Position);
#elif defined(BILLBOARD_SPHERICAL) || defined(BILLBOARD_CYLINDRICAL)
  gl_Position = gl_in[0].gl_Position;
  vec3 dir = gl_Position.xyz;
#if defined(BILLBOARD_SPHERICAL)
  vec3 right = normalize(cross(dir, vec3(0, 1, 0)));
  vec3 up = normalize(cross(right, dir));
#elif defined(BILLBOARD_CYLINDRICAL)
  vec3 up = (mtxModelView * vec4(cylindricalUpDir, 0.0)).xyz;
  vec3 right = normalize(cross(dir, up));
#endif
  gl_Position.xyz += (right * offset.x * billboardSize.x * vsScale[0].x);
  gl_Position.xyz += (up    * offset.y * billboardSize.y * vsScale[0].y);
  gl_Position = (mtxProjection * gl_Position);
#elif defined(BILLBOARD_FIXED_PX)
  vec2 billboardSizeNDC  = (billboardSizePx / viewportSize * vsScale[0]);
  gl_Position            = (mtxProjection * gl_in[0].gl_Position);
  gl_Position           /= gl_Position.w; // Perspective divide
  gl_Position.xy        += (offset * billboardSizeNDC);
#endif
  gsTexCoord = (offset * 0.5 + 0.5) * texCoordScale;
  gsColor = vsColor[0];
  EmitVertex();
}

void main()
{
  emitBillboardVert(vec2(-1, -1)); // LL
  emitBillboardVert(vec2( 1, -1)); // LR
  emitBillboardVert(vec2(-1,  1)); // UL
  emitBillboardVert(vec2( 1,  1)); // UR
  EndPrimitive();
})";

  static const char *billboardFragSrc =
R"(//#version 400 core
uniform sampler2D tex;
uniform vec3 mtlDiffuse = vec3(1.0, 1.0, 1.0);
in vec2 gsTexCoord;
in vec4 gsColor;
in vec4 vsColor;
out vec4 fsColor;
void main()
{
  vec4 texColor = texture(tex, gsTexCoord);
  if (texColor.a < 0.01) {
    discard;
  }
  fsColor = vec4(mtlDiffuse * gsColor.rgb * texColor.rgb, gsColor.a * texColor.a);
})";

  std::string header = "#version 400 core\n";
  if      (mode == BillboardMode::VIEWPLANE_SPHERICAL)   { header += "#define BILLBOARD_VIEWPLANE_SPHERICAL\n"; }
  else if (mode == BillboardMode::VIEWPLANE_CYLINDRICAL) { header += "#define BILLBOARD_VIEWPLANE_CYLINDRICAL\n"; }
  else if (mode == BillboardMode::SPHERICAL)             { header += "#define BILLBOARD_SPHERICAL\n"; }
  else if (mode == BillboardMode::CYLINDRICAL)           { header += "#define BILLBOARD_CYLINDRICAL\n"; }
  else if (mode == BillboardMode::FIXED_PX)              { header += "#define BILLBOARD_FIXED_PX\n"; }
  else { Log::warning("Unsupported billboard mode: %d", mode); }

  return Shader(header + billboardVertSrc, header + billboardGeomSrc, header + billboardFragSrc);
}

Texture Billboard::getMultidirectionalBillboardTexture(const Entity &entity)
{
  const int texWidth = 2048;
  const int texHeight = 2048;

  GLuint fbo = 0;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  GLuint depthBuf;
  glGenRenderbuffers(1, &depthBuf);
  glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texWidth, texHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

  const Texture tex(ImageFormat::RGBA8, texWidth, texHeight, nullptr, TexWrap::REPEAT, TexFilter::LINEAR, TexMipmaps::STANDARD);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex.handle, 0);

  const GLenum drawBuf[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, drawBuf);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    Log::warning("Incomplete FBO");
    return {};
  }

  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
  //glClearDepth(1.f);
  //glDepthFunc(GL_LESS);
  //glDisable(GL_DEPTH_CLAMP);

  constexpr int numLon = 8;
  constexpr int numLat = 8;

  Viewport viewport;
  viewport.width = (texWidth / numLon);
  viewport.height = (texHeight / numLat);

  for (int iLat = 0; iLat < numLat; iLat++) {
    for (int iLon = 0; iLon < numLon; iLon++) {
      viewport.x = (iLon * viewport.width);
      viewport.y = (iLat * viewport.height);

      glViewport(viewport.x, viewport.y, viewport.width, viewport.height);

      Camera cam({ 0, -10, 0 }, {});
      cam.aspectRatio = viewport.aspectRatio();
      //cam.projMode = ProjectionMode::ORTHO;
      //cam.orthoHeight = entity.bboxLocal.size().z;
      cam.buildProjMatrix();
      cam.buildViewMatrix();

      entity.draw(viewport, cam);
    }
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return tex;
}

}
