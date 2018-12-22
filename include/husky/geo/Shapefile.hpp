#pragma once

#include <husky/Common.hpp>
#include <husky/math/Vector4.hpp>
#include <vector>

namespace husky {

enum class GeometryType // TODO: Move to separate file
{
  None,
  Point,
  Line,
  Polygon,
};

class HUSKY_DLL Feature // TODO: Move to separate file
{
public:
  std::uint64_t _id;
  GeometryType _geometryType = GeometryType::None;
  Vector4d _bboxMin;
  Vector4d _bboxMax;
  std::vector<int> _parts;
  std::vector<Vector4d> _points;
};

class HUSKY_DLL FeatureTable // TODO: Move to separate file
{
public:
  GeometryType _geometryType = GeometryType::None;
  Vector4d _bboxMin;
  Vector4d _bboxMax;
  std::vector<Feature> _features;
};

class HUSKY_DLL Shapefile
{
public:
  static FeatureTable load(const std::string &shpFilePath);
};

}
