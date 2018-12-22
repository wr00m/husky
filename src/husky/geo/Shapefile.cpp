#include <husky/geo/Shapefile.hpp>
#include <fstream>
#include <WinSock2.h>

namespace husky {

enum class ShapeType : std::int32_t
{
  Null = 0,
  Point = 1,
  Polyline = 3,
  Polygon = 5,
  Multipoint = 8,
  PointZ = 11,
  PolylineZ = 13,
  PolygonZ = 15,
  MultipointZ = 18,
  PointM = 21,
  PolylineM = 23,
  PolygonM = 25,
  MultipointM = 28,
  Multipatch = 31,
};

class ShpMainFileHeader
{
public:
  std::int32_t _fileCode;
  std::int32_t _unused[5];
  std::int32_t _fileLength;
  std::int32_t _version;
  ShapeType _shapeType;
  std::double_t _xMin;
  std::double_t _yMin;
  std::double_t _xMax;
  std::double_t _yMax;
  std::double_t _zMin;
  std::double_t _zMax;
  std::double_t _mMin;
  std::double_t _mMax;
};

template<class T>
static inline T read(std::ifstream &file)
{
  T t;
  file.read(reinterpret_cast<char*>(&t), sizeof(T));
  return t;
}

FeatureTable Shapefile::load(const std::string &shpFilePath)
{
  std::ifstream file(shpFilePath, std::ios::binary);

  ShpMainFileHeader shpMainFileHeader{};
  shpMainFileHeader._fileCode = ntohl(read<std::int32_t>(file));
  read<std::int32_t>(file); // Unused
  read<std::int32_t>(file); // Unused
  read<std::int32_t>(file); // Unused
  read<std::int32_t>(file); // Unused
  read<std::int32_t>(file); // Unused
  shpMainFileHeader._fileLength = ntohl(read<std::int32_t>(file));
  shpMainFileHeader._version = read<std::int32_t>(file);
  shpMainFileHeader._shapeType = read<ShapeType>(file);
  shpMainFileHeader._xMin = read<std::double_t>(file);
  shpMainFileHeader._yMin = read<std::double_t>(file);
  shpMainFileHeader._xMax = read<std::double_t>(file);
  shpMainFileHeader._yMax = read<std::double_t>(file);
  shpMainFileHeader._zMin = read<std::double_t>(file);
  shpMainFileHeader._zMax = read<std::double_t>(file);
  shpMainFileHeader._mMin = read<std::double_t>(file);
  shpMainFileHeader._mMax = read<std::double_t>(file);

  if (shpMainFileHeader._fileCode != 9994 || shpMainFileHeader._version != 1000) {
    return {};
  }

  FeatureTable table;
  table._bboxMin.set(shpMainFileHeader._xMin, shpMainFileHeader._yMin, shpMainFileHeader._zMin, shpMainFileHeader._mMin);
  table._bboxMax.set(shpMainFileHeader._xMax, shpMainFileHeader._yMax, shpMainFileHeader._zMax, shpMainFileHeader._mMax);

  while (true) {
    Feature feature;

    feature._id = ntohl(read<std::int32_t>(file));
    std::int32_t recordContentLength = ntohl(read<std::int32_t>(file));
    std::int32_t recordContentBytes = (recordContentLength * 2);

    if (!file) {
      break;
    }

    ShapeType shapeType = read<ShapeType>(file);

    //if (shapeType != ShapeType::Null && shapeType != shpMainFileHeader._shapeType)

    if (shapeType == ShapeType::Null) {
      feature._geometryType = GeometryType::None;
    }
    // TODO: Handle more shape types
    else if (shapeType == ShapeType::Polygon) {
      feature._geometryType = GeometryType::Polygon;
      feature._bboxMin.xy = read<Vector2d>(file);
      feature._bboxMax.xy = read<Vector2d>(file);
      std::int32_t numParts = read<std::int32_t>(file);
      std::int32_t numPoints = read<std::int32_t>(file);
      feature._parts.resize(numParts);
      feature._points.reserve(numPoints);
      file.read(reinterpret_cast<char*>(feature._parts.data()), numParts * sizeof(std::int32_t));
      std::vector<Vector2d> pointsXY;
      pointsXY.resize(numPoints);
      file.read(reinterpret_cast<char*>(pointsXY.data()), numPoints * sizeof(Vector2d));
      for (const Vector2d &pointXY : pointsXY) {
        feature._points.emplace_back(pointXY.x, pointXY.y, 0.0, 0.0);
      }
    }
    else { // Unsupported shape type => Skip
      file.seekg(recordContentBytes, std::ios_base::cur);
      continue;
    }

    table._features.emplace_back(std::move(feature));
  }

  return table;
}

}
