#pragma once

#include <husky/Common.hpp>
#include <husky/Math/Vector3.hpp>
#include <string>
#include <vector>

namespace husky {

enum class HeightRef
{
  UNDEFINED,
  ELLIPSOID_WGS84,
  GEOID_EGM96,
  GEOID_EGM2008,
  GROUND,
};

class CoordSysImpl;

class HUSKY_DLL CoordSys
{
public:
  //static CoordSys wgsLonLat(HeightRef heightRef);
  //static CoordSys utm(int zone, int hemisphere, HeightRef heightRef);
  //static CoordSys geocentric();
  //static CoordSys eastNorthUp(const Vector3d &geocentricPt);

  CoordSys(int epsg);
  CoordSys(const std::string &wkt);
  ~CoordSys();

  bool sameAs(const CoordSys &other) const;

private:
  friend class CoordTransformation;
  CoordSysImpl *_p;
};

class CoordTransformationImpl;

class HUSKY_DLL CoordTransformation
{
public:
  CoordTransformation(const CoordSys &src, const CoordSys &dst);
  ~CoordTransformation();

  bool convert(Vector3d &pt) const;
  int convert(std::vector<Vector3d> &pts) const;

private:
  CoordTransformationImpl *_p;
};

}
