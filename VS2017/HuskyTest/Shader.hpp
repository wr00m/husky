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
    mtxModelViewLocation          = glGetUniformLocation(shaderProgram, "mtxModelView");
    mtxNormalLocation             = glGetUniformLocation(shaderProgram, "mtxNormal");
    mtxProjectionLocation         = glGetUniformLocation(shaderProgram, "mtxProjection");
    texLocation                   = glGetUniformLocation(shaderProgram, "tex");
    lightDirLocation              = glGetUniformLocation(shaderProgram, "lightDir");
    mtlAmbientLocation            = glGetUniformLocation(shaderProgram, "mtlAmbient");
    mtlDiffuseLocation            = glGetUniformLocation(shaderProgram, "mtlDiffuse");
    mtlSpecularLocation           = glGetUniformLocation(shaderProgram, "mtlSpecular");
    mtlEmissiveLocation           = glGetUniformLocation(shaderProgram, "mtlEmissive");
    mtlShininessLocation          = glGetUniformLocation(shaderProgram, "mtlShininess");
    mtlShininessStrengthLocation  = glGetUniformLocation(shaderProgram, "mtlShininessStrength");
    viewportSizeLocation          = glGetUniformLocation(shaderProgram, "viewportSize");
    lineWidthLocation             = glGetUniformLocation(shaderProgram, "lineWidth");

    vertPositionLocation          = glGetAttribLocation(shaderProgram, "vPosition");
    vertNormalLocation            = glGetAttribLocation(shaderProgram, "vNormal");
    vertTexCoordLocation          = glGetAttribLocation(shaderProgram, "vTexCoord");
    vertColorLocation             = glGetAttribLocation(shaderProgram, "vColor");
  }

  GLuint shaderProgram;
  GLint mtxModelViewLocation;
  GLint mtxNormalLocation;
  GLint mtxProjectionLocation;
  GLint texLocation;
  GLint lightDirLocation;
  GLint mtlAmbientLocation;
  GLint mtlDiffuseLocation;
  GLint mtlSpecularLocation;
  GLint mtlEmissiveLocation;
  GLint mtlShininessLocation;
  GLint mtlShininessStrengthLocation;
  GLint viewportSizeLocation;
  GLint lineWidthLocation;
  GLint vertPositionLocation;
  GLint vertNormalLocation;
  GLint vertTexCoordLocation;
  GLint vertColorLocation;
  GLuint textureHandle; // TODO: Move to Material
};
