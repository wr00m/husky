#include <husky/render/Billboard.hpp>
#include <husky/Log.hpp>

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
  vec3 right = normalize(cross(dir, vec3(0, 1, 0)));
#if defined(BILLBOARD_SPHERICAL)
  vec3 up = normalize(cross(right, dir));
#elif defined(BILLBOARD_CYLINDRICAL)
  vec3 up = (mtxModelView * vec4(cylindricalUpDir, 0.0)).xyz;
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

Texture Billboard::getMultidirectionalBillboardTexture(const Model &mdl)
{
  return{};
}

}
