#include <husky/render/Billboard.hpp>
#include <husky/math/EulerAngles.hpp>
#include <husky/math/Random.hpp>
#include <husky/math/Math.hpp>
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
  gl_Position = (mtxModelView * vec4(vertPosition, 1.0));
})";

  static const char *billboardGeomSrc =
R"(//#version 400 core
#define M_PI 3.14159265358979323846
uniform mat4 mtxModelView;
//uniform mat3 mtxNormal;
uniform mat4 mtxProjection;
uniform mat3 mtxLocalOrientation // mtxNormal? Model or world coordinates?
  = mat3(1, 0, 0,  // Right
         0, 1, 0,  // Forward
         0, 0, 1); // Up
uniform vec2 numSubTextures = vec2(64, 63); // Integer
uniform vec2 viewportSize = vec2(1280, 720); // Pixels
#if defined(BILLBOARD_FIXED_PX)
uniform vec2 billboardSize = vec2(64, 64); // Pixels
#else
uniform vec2 billboardSize = vec2(1, 1); // World units
#endif
in vec2 vsScale[1];
in vec4 vsColor[1];
out vec2 gsTexCoord;
out vec4 gsColor;
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

float angleSigned(const vec3 a, const vec3 b, const vec3 axis)
{
  float cosTheta = dot(a, b);
  float theta = acos(cosTheta); // [0,pi]
  float sinTheta = dot(cross(axis, a), b);
  if (sinTheta < 0) {
    theta = -theta;
  }
  return theta;
}

//float angleAbs(const vec3 a, const vec3 b)
//{
//  float cosTheta = dot(a, b);
//  float theta = acos(cosTheta); // [0,pi]
//  return theta;
//}

void emitVertFinalize(const vec2 offset)
{
  //emitVertFinalize(offset, vec4(0, 0, 1, 1));
}

void emitVertFinalize(const vec2 offset, const vec4 subTexBounds)
{
  gsTexCoord = 0.5 + 0.5 * offset; // Subtexture UV; [0,1]
  gsTexCoord = mix(subTexBounds.xy, subTexBounds.zw, gsTexCoord); // Texture UV; [0,1]
  gsTexCoord.y = (1.0 - gsTexCoord.y); // Flip V
  gsColor = vsColor[0];
  EmitVertex();
}

void emitVertViewplaneSpherical(const vec2 offset)
{
  gl_Position = gl_in[0].gl_Position;
  gl_Position.xy += (offset * billboardSize * vsScale[0]);
  gl_Position = (mtxProjection * gl_Position);
  emitVertFinalize(offset);
}

void emitVertViewplaneCylindrical(const vec2 offset, const vec3 up)
{
  gl_Position = gl_in[0].gl_Position;
  gl_Position.x += (offset.x * billboardSize.x * vsScale[0].x);
  gl_Position.xyz += (up * offset.y * billboardSize.y * vsScale[0].y);
  gl_Position = (mtxProjection * gl_Position);
  emitVertFinalize(offset);
}

void emitVert(const vec2 offset, const vec3 right, const vec3 up, const vec4 subTexBounds)
{
  gl_Position = gl_in[0].gl_Position;
  gl_Position.xyz += (right * offset.x * billboardSize.x * vsScale[0].x);
  gl_Position.xyz += (up    * offset.y * billboardSize.y * vsScale[0].y);
  gl_Position = (mtxProjection * gl_Position);
  emitVertFinalize(offset, subTexBounds);
}

void emitVertFixedSize(const vec2 offset, const vec2 billboardSizeNDC)
{
  gl_Position = (mtxProjection * gl_in[0].gl_Position);
  gl_Position /= gl_Position.w; // Perspective divide
  gl_Position.xy += (offset * billboardSizeNDC);
  emitVertFinalize(offset);
}

void main()
{
  const vec2 ll = vec2(-1, -1);
  const vec2 lr = vec2( 1, -1);
  const vec2 ul = vec2(-1,  1);
  const vec2 ur = vec2( 1,  1);

  mat3 mtxLocalOrientationVS = mat3(mtxModelView) * mtxLocalOrientation;
  vec3 localRightVS = mtxLocalOrientationVS[0];
  //vec3 localForwardVS = mtxLocalOrientationVS[1];
  vec3 localUpVS = mtxLocalOrientationVS[2];

#if defined(BILLBOARD_VIEWPLANE_SPHERICAL)
  emitVertViewplaneSpherical(ll);
  emitVertViewplaneSpherical(lr);
  emitVertViewplaneSpherical(ul);
  emitVertViewplaneSpherical(ur);
#endif

#if defined(BILLBOARD_VIEWPLANE_CYLINDRICAL)
  vec3 up = localUpVS;
  emitVertViewplaneCylindrical(ll, up);
  emitVertViewplaneCylindrical(lr, up);
  emitVertViewplaneCylindrical(ul, up);
  emitVertViewplaneCylindrical(ur, up);
#endif

#if defined(BILLBOARD_SPHERICAL)
  vec3 dir = gl_in[0].gl_Position.xyz;
  vec3 right = normalize(cross(dir, vec3(0, 1, 0)));
  vec3 up = normalize(cross(right, dir));

  // Note: This yaw/pitch calculation is a bit off, but will do for now...
  float yaw = angleSigned(right, localRightVS, vec3(0, 1, 0)); // [-pi,pi]
  float nYaw = 0.5 - yaw / (M_PI * 2); // [0,1]
  float pitch = angleSigned(up, localUpVS, right); // [-pi,pi]
  float nPitch = -0.25 + pitch / (M_PI * 2); // [0,1]

  vec2 subTexIndex = vec2(nYaw, nPitch); // [0,1]
  subTexIndex *= numSubTextures; // [0,numSubTextures]
  vec4 subTexBounds = floor(subTexIndex.xy).xyxy + vec4(0, 0, 1, 1); // Integer; [0,numSubTextures]
  subTexBounds /= numSubTextures.xyxy; // [0,1]
  emitVert(ll, right, up, subTexBounds);
  emitVert(lr, right, up, subTexBounds);
  emitVert(ul, right, up, subTexBounds);
  emitVert(ur, right, up, subTexBounds);
#endif

#if defined(BILLBOARD_CYLINDRICAL)
  vec3 dir = gl_in[0].gl_Position.xyz;
  vec3 up = localUpVS;
  vec3 right = normalize(cross(dir, up));
  emitVert(ll, right, up);
  emitVert(lr, right, up);
  emitVert(ul, right, up);
  emitVert(ur, right, up);
#endif

#if defined(BILLBOARD_FIXED_PX)
  vec2 billboardSizeNDC  = (billboardSize / viewportSize * vsScale[0]); // Pixels to normalized device coordinates
  emitVertFixedSize(ll, billboardSizeNDC);
  emitVertFixedSize(lr, billboardSizeNDC);
  emitVertFixedSize(ul, billboardSizeNDC);
  emitVertFixedSize(ur, billboardSizeNDC);
#endif

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

static void drawFullscreenQuad(const Vector3f &color)
{
  static const char *fsqVertSrc =
R"(#version 400 core
in vec3 vertPosition;
void main() {
  gl_Position = vec4(vertPosition, 1.0);
})";

  static const char *fsqFragSrc =
R"(#version 400 core
uniform vec3 mtlDiffuse = vec3(1.0, 1.0, 1.0);
out vec4 fragColor;
void main() {
  fragColor = vec4(mtlDiffuse, 1.0);
})";

  static const Shader fsqShader(fsqVertSrc, "", fsqFragSrc);

  Mesh mesh;
  mesh.addQuad({ -1, -1, 0 }, { 1, -1, 0 }, { 1, 1, 0 }, { -1, 1, 0 });

  Material mtl(color);
  mtl.twoSided = true;
  mtl.depthTest = false;

  RenderData rd = mesh.getRenderData();
  rd.draw(fsqShader, mtl, {}, {}, {}, {}, {});
}

MultidirTexture Billboard::getMultidirectionalBillboardTexture(const Entity &entity, int texWidth, int texHeight, int numLon, int numLat)
{
  GLuint fbo = 0;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  GLuint depthBuf;
  glGenRenderbuffers(1, &depthBuf);
  glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texWidth, texHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

  MultidirTexture mdTex(Texture(ImageFormat::RGBA8, texWidth, texHeight, TexWrap::REPEAT, TexFilter::LINEAR, TexMipmaps::STANDARD, nullptr), numLon, numLat);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mdTex.tex.handle, 0);

  const GLenum drawBuf[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, drawBuf);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    Log::warning("Incomplete FBO");
    return mdTex;
  }

  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
  //glClearDepth(1.f);
  //glDepthFunc(GL_LESS);
  //glDisable(GL_DEPTH_CLAMP);

  Viewport viewport;
  viewport.width = (texWidth / numLon);
  viewport.height = (texHeight / numLat);

  Camera cam(entity.bsphereLocal.center, {});
  cam.orthoHeight = (entity.bsphereLocal.radius * 2);
  cam.projMode = ProjectionMode::ORTHO;
  cam.aspectRatio = viewport.aspectRatio();
  cam.buildProjMatrix();

  Random random;

  for (int iLat = 0; iLat < numLat; iLat++) {
    double v = (iLat / (double)(numLat - 1));
    double latRad = (-v * Math::twoPi);
    viewport.y = (iLat * viewport.height);

    for (int iLon = 0; iLon < numLon; iLon++) {
      double u = (iLon / (double)numLon);
      double lonRad = -(Math::pi + u * Math::twoPi);
      viewport.x = (iLon * viewport.width);

      EulerAnglesd eulerAngles(RotationOrder::ZXY, lonRad, latRad, 0);
      cam.rot = eulerAngles.toQuaternion();
      cam.pos = entity.getTransform().col[3].xyz; // Center view on Entity
      cam.pos -= (cam.forward() * entity.modelInstance.model->bsphereLocal.radius);
      cam.buildViewMatrix();

      glViewport(viewport.x, viewport.y, viewport.width, viewport.height);

      //Vector3d color(random.getDouble(0.8, 1.0), random.getDouble(0.8, 1.0), random.getDouble(0.8, 1.0));
      //drawFullscreenQuad((Vector3f)color);

      entity.draw(viewport, cam);
    }
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  mdTex.tex.buildMipmaps();

  return mdTex;
}

}
