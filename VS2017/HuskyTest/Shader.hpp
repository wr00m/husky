#pragma once

#include <glad/glad.h>
#include <map>

class Shader
{
public:
  Shader()
    : Shader(0)
  {
  }

  Shader(GLuint shaderProgram)
    : shaderProgram(shaderProgram)
  {
    GLint varSize;
    GLenum varType;
    static constexpr GLsizei varNameLengthMax = 128;
    GLchar varName[varNameLengthMax];
    GLsizei varNameLength;

    // Get active uniforms
    int uniformCount;
    glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &uniformCount);
    for (int iUniform = 0; iUniform < uniformCount; iUniform++) {
      glGetActiveUniform(shaderProgram, (GLuint)iUniform, varNameLengthMax, &varNameLength, &varSize, &varType, varName);
      uniformLocations[varName] = glGetUniformLocation(shaderProgram, varName);
    }

    // Get active (vertex) attributes
    int attrCount;
    glGetProgramiv(shaderProgram, GL_ACTIVE_ATTRIBUTES, &attrCount);
    for (int iAttr = 0; iAttr < attrCount; iAttr++) {
      glGetActiveAttrib(shaderProgram, (GLuint)iAttr, varNameLengthMax, &varNameLength, &varSize, &varType, varName);
      attrLocations[varName] = glGetAttribLocation(shaderProgram, varName);
    }
  }

  bool getUniformLocation(const std::string &uniformName, int &location) const
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

  bool getAttributeLocation(const std::string &attrName, int &location) const
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

  GLuint shaderProgram;
  std::map<const std::string, GLint> uniformLocations;
  std::map<const std::string, GLint> attrLocations;
};
