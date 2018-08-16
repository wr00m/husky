#pragma once

#include <husky/Common.hpp>
#include <map>

namespace husky {

enum class BillboardMode
{
  VIEWPLANE_SPHERICAL,
  VIEWPLANE_CYLINDRICAL,
  SPHERICAL,
  //CYLINDRICAL,
  FIXED_PX,
};

class HUSKY_DLL Shader
{
public:
  static Shader getDefaultShader(bool texture, bool bones);
  static Shader getLineShader();
  static Shader getBillboardShader(BillboardMode mode);

  Shader();
  Shader(unsigned int shaderProgramHandle);
  Shader(const std::string &vertSrc, const std::string &geomSrc, const std::string &fragSrc);

  bool getUniformLocation(const std::string &uniformName, int &location) const;
  bool getAttributeLocation(const std::string &attrName, int &location) const;

  unsigned int shaderProgramHandle;
  std::map<const std::string, int> uniformLocations;
  std::map<const std::string, int> attrLocations;
};

}
