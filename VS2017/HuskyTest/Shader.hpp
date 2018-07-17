#pragma once

#include <glad/glad.h>

class Shader
{
public:
  Shader()
    : Shader(0)
  {
  }

  Shader(GLuint shaderProgram)
    : shaderProgram(shaderProgram)
    , textureHandle(0)
  {
    modelViewLocation         = glGetUniformLocation(shaderProgram, "modelView");
    projectionLocation        = glGetUniformLocation(shaderProgram, "projection");
    texLocation               = glGetUniformLocation(shaderProgram, "tex");
    diffuseLightColorLocation = glGetUniformLocation(shaderProgram, "diffuseLightColor");
    ambientLightColorLocation = glGetUniformLocation(shaderProgram, "ambientLightColor");
    viewportSizeLocation      = glGetUniformLocation(shaderProgram, "viewportSize");
    lineWidthLocation         = glGetUniformLocation(shaderProgram, "lineWidth");

    vertPositionLocation      = glGetAttribLocation(shaderProgram, "vPosition");
    vertNormalLocation        = glGetAttribLocation(shaderProgram, "vNormal");
    vertTexCoordLocation      = glGetAttribLocation(shaderProgram, "vTexCoord");
    vertColorLocation         = glGetAttribLocation(shaderProgram, "vColor");
  }

  GLuint shaderProgram;
  GLint modelViewLocation;
  GLint projectionLocation;
  GLint texLocation;
  GLint diffuseLightColorLocation;
  GLint ambientLightColorLocation;
  GLint viewportSizeLocation;
  GLint lineWidthLocation;
  GLint vertPositionLocation;
  GLint vertNormalLocation;
  GLint vertTexCoordLocation;
  GLint vertColorLocation;
  GLuint textureHandle; // TODO: Move to Material
};
