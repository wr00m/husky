#include <husky/geo/CoordSys.hpp>
#include <husky/Log.hpp>
#include <gdal.h>
#include <ogr_spatialref.h>

namespace husky {

static void initOgr()
{
  static bool ogrInitialized = false;
  if (!ogrInitialized) {
    ogrInitialized = true;
    OGRRegisterAll();
  }
}

class CoordSysImpl
{
public:
  CoordSysImpl() { initOgr(); _osr = new OGRSpatialReference; }
  //~CoordSysImpl() { OGRSpatialReference::DestroySpatialReference(_osr); } // TODO: Why do we get an exception here?

  OGRSpatialReference *_osr;
};

CoordSys::CoordSys(int epsg)
  : _p(new CoordSysImpl)
{
  _p->_osr->importFromEPSG(epsg);

  if (_p->_osr->Validate() != OGRERR_NONE) {
    Log::warning("Failed to create coordinate system from EPSG: %d", epsg);
  }
}

CoordSys::CoordSys(const std::string &wkt)
  : _p(new CoordSysImpl)
{
  _p->_osr->importFromWkt(wkt.c_str());

  if (_p->_osr->Validate() != OGRERR_NONE) {
    Log::warning("Failed to create coordinate system from WKT: %s", wkt.c_str());
  }
}

CoordSys::~CoordSys()
{
  delete _p;
}

bool CoordSys::sameAs(const CoordSys &other) const
{
  return (_p->_osr->IsSame(other._p->_osr) == TRUE);
}

class CoordTransformationImpl
{
public:
  CoordTransformationImpl(OGRSpatialReference *src, OGRSpatialReference *dst) { initOgr(); _oct = OGRCreateCoordinateTransformation(src, dst); }
  ~CoordTransformationImpl() { OGRCoordinateTransformation::DestroyCT(_oct); }

  OGRCoordinateTransformation *_oct;
};

CoordTransformation::CoordTransformation(const CoordSys &src, const CoordSys &dst)
  : _p(new CoordTransformationImpl(src._p->_osr, dst._p->_osr))
{
}

CoordTransformation::~CoordTransformation()
{
  delete _p;
}

bool CoordTransformation::convert(Vector3d &pt) const
{
  return (_p->_oct->Transform(1, &pt.x, &pt.y, &pt.z) == TRUE);
}

int CoordTransformation::convert(std::vector<Vector3d> &pts) const
{
  std::vector<double> x, y, z;
  x.reserve(pts.size());
  y.reserve(pts.size());
  z.reserve(pts.size());
  for (const Vector3d &pt : pts) {
    x.emplace_back(pt.x);
    y.emplace_back(pt.y);
    z.emplace_back(pt.z);
  }

  return (_p->_oct->Transform((int)pts.size(), x.data(), y.data(), z.data()) == TRUE);
}

}
