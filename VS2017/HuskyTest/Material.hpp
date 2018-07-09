#pragma once

#include <glad/glad.h>

class Material
{
public:
  Material()
    : Material(0)
  {
  }

  Material(GLuint shaderProgram)
    : shaderProgram(shaderProgram)
    , textureHandle(0)
    , lineWidth(2)
  {
    modelViewLocation     = glGetUniformLocation(shaderProgram, "modelView");
    projectionLocation    = glGetUniformLocation(shaderProgram, "projection");
    texLocation           = glGetUniformLocation(shaderProgram, "tex");
    viewportSizeLocation  = glGetUniformLocation(shaderProgram, "viewportSize");
    lineWidthLocation     = glGetUniformLocation(shaderProgram, "lineWidth");

    vertPositionLocation  = glGetAttribLocation(shaderProgram, "vPosition");
    vertNormalLocation    = glGetAttribLocation(shaderProgram, "vNormal");
    vertTexCoordLocation  = glGetAttribLocation(shaderProgram, "vTexCoord");
    vertColorLocation     = glGetAttribLocation(shaderProgram, "vColor");
  }

  GLuint shaderProgram;
  GLint modelViewLocation;
  GLint projectionLocation;
  GLint texLocation;
  GLint viewportSizeLocation;
  GLint lineWidthLocation;
  GLint vertPositionLocation;
  GLint vertNormalLocation;
  GLint vertTexCoordLocation;
  GLint vertColorLocation;
  GLuint textureHandle;
  float lineWidth;
};
