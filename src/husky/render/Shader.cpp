#pragma once

#include <husky/render/Shader.hpp>
#include <husky/Log.hpp>
#include <glad/glad.h>
#include <vector>

namespace husky {

static GLuint compileShader(GLenum shaderType, const std::string &shaderSrc)
{
  GLuint shader = glCreateShader(shaderType);
  const char *cShaderSrc = shaderSrc.c_str();
  glShaderSource(shader, 1, &cShaderSrc, NULL);
  glCompileShader(shader);

  GLint isCompiled = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    GLint logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

    std::vector<GLchar> log(logLength, ' ');
    glGetShaderInfoLog(shader, logLength, nullptr, log.data());

    std::string s(log.begin(), log.end());
    Log::error(s.c_str());

    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

static GLuint compileShaderProgram(const std::string &vertSrc, const std::string &geomSrc, const std::string &fragSrc)
{
  GLuint program = glCreateProgram();

  GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertSrc);
  glAttachShader(program, vertShader);

  if (!geomSrc.empty()) {
    GLuint geomShader = compileShader(GL_GEOMETRY_SHADER, geomSrc);
    glAttachShader(program, geomShader);
  }

  GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragSrc);
  glAttachShader(program, fragShader);

  glLinkProgram(program);

  GLint isLinked = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
  if (isLinked == GL_FALSE) {
    GLint logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

    std::vector<GLchar> log(logLength);
    glGetProgramInfoLog(program, logLength, nullptr, log.data());

    std::string s(log.begin(), log.end());
    Log::error(s.c_str());

    glDeleteProgram(program);
    return 0;
  }

  return program;
}

Shader Shader::getDefaultShader(bool texture, bool bones)
{
  static const char *defaultVertSrc =
R"(//#version 400 core
#ifdef USE_BONES
#ifndef MAX_BONES
#define MAX_BONES 100 // Note: We use 8-bit bone indices, so use MAX_BONES <= 256
#endif
uniform mat4 mtxBones[MAX_BONES] = mat4[MAX_BONES](mat4(1.0));
in ivec4 vertBoneIndices;
in vec4 vertBoneWeights;
#endif
uniform mat4 mtxModelView;
uniform mat3 mtxNormal;
uniform mat4 mtxProjection;
uniform vec2 texCoordScale = vec2(1.0, -1.0); // Flip vertically
in vec3 vertPosition;
in vec3 vertNormal;
in vec2 vertTexCoord;
in vec4 vertColor;
out vec4 varPosition;
out vec3 varNormal;
out vec2 varTexCoord;
out vec4 varColor;
void main() {
#ifdef USE_BONES
  mat4 mtxBone = mtxBones[vertBoneIndices[0]] * vertBoneWeights[0]
               + mtxBones[vertBoneIndices[1]] * vertBoneWeights[1]
               + mtxBones[vertBoneIndices[2]] * vertBoneWeights[2]
               + mtxBones[vertBoneIndices[3]] * vertBoneWeights[3];
  varPosition = mtxModelView * mtxBone * vec4(vertPosition, 1.0);
  varNormal = normalize(mtxNormal * (mtxBone * vec4(vertNormal, 0.0)).xyz);
#else
  varPosition = mtxModelView * vec4(vertPosition, 1.0);
  varNormal = normalize(mtxNormal * vertNormal);
#endif
  varTexCoord = vertTexCoord * texCoordScale;
  varColor = vertColor;
  gl_Position = mtxProjection * varPosition;
})";

  static const char *defaultFragSrc =
R"(//#version 400 core
#ifdef USE_TEXTURE
uniform sampler2D tex;
#endif
uniform vec3 lightDir = vec3(20.0, -40.0, 100.0);
uniform vec3 lightAmbient = vec3(0.05, 0.05, 0.05);
uniform vec3 lightDiffuse = vec3(1.0, 1.0, 1.0);
uniform vec3 lightSpecular = vec3(1.0, 1.0, 1.0);
uniform vec3 mtlAmbient = vec3(1.0, 1.0, 1.0);
uniform vec3 mtlDiffuse = vec3(1.0, 1.0, 1.0);
uniform vec3 mtlSpecular = vec3(1.0, 1.0, 1.0);
uniform vec3 mtlEmissive = vec3(0.0, 0.0, 0.0);
uniform float mtlShininess = 100.0;
uniform float mtlShininessStrength = 1.0;
in vec4 varPosition;
in vec3 varNormal;
in vec2 varTexCoord;
in vec4 varColor;
out vec4 fragColor;
void main() {
  vec3 v = varPosition.xyz;
  vec3 N = varNormal;
  vec3 L = lightDir;
  vec3 E = normalize(-v);
  vec3 R = normalize(-reflect(L, N));
  vec3 ambientColor = (lightAmbient * mtlAmbient);
  float diffuseIntensity = clamp(dot(N, L), 0.0, 1.0);
  vec3 diffuseColor = diffuseIntensity * lightDiffuse * mtlDiffuse;
  float specularIntensity = clamp(pow(max(dot(R, E), 0.0), mtlShininess) * mtlShininessStrength, 0.0, 1.0);
  vec3 specularColor = specularIntensity * lightSpecular * mtlSpecular;
#ifdef USE_TEXTURE
  vec4 texColor = texture(tex, varTexCoord);
#else
  const vec4 texColor = vec4(1.0);
#endif
  //texColor = vec4(1.0);
  fragColor.rgb = ambientColor + (diffuseColor * varColor.rgb * texColor.rgb) + specularColor + mtlEmissive;
  fragColor.a = varColor.a * texColor.a;
})";

  std::string header = "#version 400 core\n";
  if (texture) { header += "#define USE_TEXTURE\n"; }
  if (bones) { header += "#define USE_BONES\n"; }
  
  return Shader(header + defaultVertSrc, "", header + defaultFragSrc);
}

Shader Shader::getLineShader()
{
  static const char *lineVertSrc =
R"(#version 400 core
uniform mat4 mtxModelView;
uniform mat4 mtxProjection;
in vec3 vertPosition;
in vec4 vertColor;
out vec4 vsColor;
void main()
{
  vsColor = vertColor;
  gl_Position = mtxProjection * (mtxModelView * vec4(vertPosition, 1.0));
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
uniform vec3 mtlDiffuse = vec3(1.0, 1.0, 1.0);
in vec4 gsColor;
out vec4 fsColor;
void main()
{
  fsColor = gsColor * vec4(mtlDiffuse, 1.0);
})";

  return Shader(lineVertSrc, lineGeomSrc, lineFragSrc);
}

Shader Shader::getBillboardShader(BillboardMode mode)
{
  static const char *billboardVertSrc =
R"(//#version 400 core
uniform mat4 mtxModelView;
in vec3 vertPosition;
in vec4 vertColor;
out vec4 vsColor;
void main()
{
  vsColor = vertColor;
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
#if defined(BILLBOARD_CYLINDRICAL)
uniform vec3 cylindricalUpDir = vec3(0, 0, 1);
#endif
uniform vec2 billboardSize = vec2(1, 1); // World units
#endif
in vec4 vsColor[1];
out vec2 gsTexCoord;
out vec4 gsColor;
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

void emitBillboardVert(const vec2 offset)
{
#if defined(BILLBOARD_VIEWPLANE)
  gl_Position = gl_in[0].gl_Position;
  gl_Position.xy += (offset * billboardSize);
  gl_Position = (mtxProjection * gl_Position);
#elif defined(BILLBOARD_CYLINDRICAL)
  vec3 up = (mtxModelView * vec4(cylindricalUpDir, 0.0)).xyz;
  gl_Position = gl_in[0].gl_Position;
  gl_Position.x += (offset.x * billboardSize.x);
  gl_Position.xyz += (up * offset.y * billboardSize.y);
  gl_Position = (mtxProjection * gl_Position);
#elif defined(BILLBOARD_SPHERICAL)
  gl_Position = gl_in[0].gl_Position;
  vec3 dir = gl_Position.xyz;
  vec3 right = normalize(cross(dir, vec3(0, 1, 0)));
  vec3 up = normalize(cross(right, dir));
  gl_Position.xyz += (right * offset.x * billboardSize.x);
  gl_Position.xyz += (up    * offset.y * billboardSize.y);
  gl_Position = (mtxProjection * gl_Position);
#elif defined(BILLBOARD_FIXED_PX)
  vec2 billboardSizeNDC  = (billboardSizePx / viewportSize);
  gl_Position            = (mtxProjection * gl_in[0].gl_Position);
  gl_Position           /= gl_Position.w; // Perspective divide
  gl_Position.xy        += (offset * billboardSizeNDC);
#endif
  gsTexCoord = (offset * 0.5 + 0.5);
  gsColor = vsColor[0];
  EmitVertex();
}

void main()
{
  emitBillboardVert(vec2(-1, -1)); // LL
  emitBillboardVert(vec2(-1,  1)); // UL
  emitBillboardVert(vec2( 1, -1)); // LR
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
  fsColor = vec4(mtlDiffuse * gsColor.rgb * texColor.rgb, gsColor.a * texColor.a);
})";

  std::string header = "#version 400 core\n";
  if      (mode == BillboardMode::VIEWPLANE)   { header += "#define BILLBOARD_VIEWPLANE\n"; }
  else if (mode == BillboardMode::SPHERICAL)   { header += "#define BILLBOARD_SPHERICAL\n"; }
  else if (mode == BillboardMode::CYLINDRICAL) { header += "#define BILLBOARD_CYLINDRICAL\n"; }
  else if (mode == BillboardMode::FIXED_PX)    { header += "#define BILLBOARD_FIXED_PX\n"; }
  else { Log::warning("Unsupported billboard mode: ", mode); }

  return Shader(header + billboardVertSrc, header + billboardGeomSrc, header + billboardFragSrc);
}

Shader::Shader()
  : Shader(0)
{
}

Shader::Shader(unsigned int shaderProgramHandle)
  : shaderProgramHandle(shaderProgramHandle)
{
  GLint varSize;
  GLenum varType;
  static constexpr GLsizei varNameLengthMax = 128;
  GLchar varName[varNameLengthMax];
  GLsizei varNameLength;

  // Get active uniforms
  int uniformCount;
  glGetProgramiv(shaderProgramHandle, GL_ACTIVE_UNIFORMS, &uniformCount);
  for (int iUniform = 0; iUniform < uniformCount; iUniform++) {
    glGetActiveUniform(shaderProgramHandle, (GLuint)iUniform, varNameLengthMax, &varNameLength, &varSize, &varType, varName);
    std::string uniformName = varName;
    size_t b = uniformName.find('['); // Array uniform
    if (b != std::string::npos) {
      uniformName = uniformName.substr(0, b);
    }
    uniformLocations[uniformName] = glGetUniformLocation(shaderProgramHandle, varName);
  }

  // Get active (vertex) attributes
  int attrCount;
  glGetProgramiv(shaderProgramHandle, GL_ACTIVE_ATTRIBUTES, &attrCount);
  for (int iAttr = 0; iAttr < attrCount; iAttr++) {
    glGetActiveAttrib(shaderProgramHandle, (GLuint)iAttr, varNameLengthMax, &varNameLength, &varSize, &varType, varName);
    attrLocations[varName] = glGetAttribLocation(shaderProgramHandle, varName);
  }
}

Shader::Shader(const std::string &vertSrc, const std::string &geomSrc, const std::string &fragSrc)
  : Shader(compileShaderProgram(vertSrc, geomSrc, fragSrc))
{
}

bool Shader::getUniformLocation(const std::string &uniformName, int &location) const
{
  auto it = uniformLocations.find(uniformName);
  if (it != uniformLocations.end()) {
    location = it->second;
    return true;
  }
  else {
    location = -1;
    return false;
  }
}

bool Shader::getAttributeLocation(const std::string &attrName, int &location) const
{
  auto it = attrLocations.find(attrName);
  if (it != attrLocations.end()) {
    location = it->second;
    return true;
  }
  else {
    location = -1;
    return false;
  }
}

}
