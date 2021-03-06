#include <husky/render/Shader.hpp>
#include <husky/Log.hpp>
#include <glad/glad.h>
#include <vector>

// TODO: Use in/out interface blocks: https://www.opengl.org/discussion_boards/showthread.php/185141-Is-there-a-way-that-Geometryshaders-in-and-out-variables-have-the-same-name

namespace husky {

ShaderUniform::ShaderUniform()
  : ShaderUniform("", -1, -1, -1)
{
}

ShaderUniform::ShaderUniform(const std::string &name, int location, int type, int size)
  : name(name)
  , location(location)
  , type(type)
  , size(size)
{
}

ShaderUniform::operator bool() const
{
  return (location != -1);
}

ShaderAttribute::ShaderAttribute()
  : ShaderAttribute("", -1, -1, -1)
{
}

ShaderAttribute::ShaderAttribute(const std::string &name, int location, int type, int size)
  : name(name)
  , location(location)
  , type(type)
  , size(size)
{
}

ShaderAttribute::operator bool() const
{
  return (location != -1);
}

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
uniform bool useBones = true; // TODO: Rewrite shader to avoid if-else branching
in ivec4 vertBoneIndices;
in vec4 vertBoneWeights;
#endif
uniform mat4 mtxModelView;
uniform mat3 mtxNormal;
uniform mat4 mtxProjection;
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
  mat4 mtxBone = mat4(1.0);
  if (useBones) {
    mtxBone = mtxBones[vertBoneIndices[0]] * vertBoneWeights[0]
            + mtxBones[vertBoneIndices[1]] * vertBoneWeights[1]
            + mtxBones[vertBoneIndices[2]] * vertBoneWeights[2]
            + mtxBones[vertBoneIndices[3]] * vertBoneWeights[3];
  }
  varPosition = mtxModelView * mtxBone * vec4(vertPosition, 1.0);
  varNormal = normalize(mtxNormal * (mtxBone * vec4(vertNormal, 0.0)).xyz);
#else
  varPosition = mtxModelView * vec4(vertPosition, 1.0);
  varNormal = normalize(mtxNormal * vertNormal);
#endif
  varTexCoord = vec2(vertTexCoord.x, 1.0 - vertTexCoord.y); // Flip V
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
    uniforms.emplace_back(uniformName, glGetUniformLocation(shaderProgramHandle, varName), varType, varSize);
  }

  // Get active (vertex) attributes
  int attrCount;
  glGetProgramiv(shaderProgramHandle, GL_ACTIVE_ATTRIBUTES, &attrCount);
  for (int iAttr = 0; iAttr < attrCount; iAttr++) {
    glGetActiveAttrib(shaderProgramHandle, (GLuint)iAttr, varNameLengthMax, &varNameLength, &varSize, &varType, varName);
    attrs.emplace_back(varName, glGetAttribLocation(shaderProgramHandle, varName), varType, varSize);
  }
}

Shader::Shader(const std::string &vertSrc, const std::string &geomSrc, const std::string &fragSrc)
  : Shader(compileShaderProgram(vertSrc, geomSrc, fragSrc))
{
}

const ShaderUniform& Shader::getUniform(const std::string &uniformName) const
{
  for (const ShaderUniform &uniform : uniforms) {
    if (uniform.name == uniformName) {
      return uniform;
    }
  }
  static const ShaderUniform empty;
  return empty;
}

const ShaderAttribute& Shader::getAttribute(const std::string &attrName) const
{
  for (const ShaderAttribute &attr : attrs) {
    if (attr.name == attrName) {
      return attr;
    }
  }
  static const ShaderAttribute empty;
  return empty;
}

}
