#pragma once

#include <husky/Common.hpp>
#include <string>
#include <vector>

namespace husky {

class HUSKY_DLL ShaderUniform
{
public:
  ShaderUniform();
  ShaderUniform(const std::string &name, int location, int type, int size);

  operator bool() const;

  std::string name;
  int location;
  int type; // TODO: Don't store raw OpenGL value
  int size;
};

class HUSKY_DLL ShaderAttribute
{
public:
  ShaderAttribute();
  ShaderAttribute(const std::string &name, int location, int type, int size);

  operator bool() const;

  std::string name;
  int location;
  int type; // TODO: Don't store raw OpenGL value
  int size;
};

class HUSKY_DLL Shader
{
public:
  static Shader getDefaultShader(bool texture, bool bones);
  static Shader getLineShader();

  Shader();
  Shader(unsigned int shaderProgramHandle);
  Shader(const std::string &vertSrc, const std::string &geomSrc, const std::string &fragSrc);

  const ShaderUniform& getUniform(const std::string &uniformName) const;
  const ShaderAttribute& getAttribute(const std::string &attrName) const;

  unsigned int shaderProgramHandle;
  std::vector<ShaderUniform> uniforms;
  std::vector<ShaderAttribute> attrs;
};

}
