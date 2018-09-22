#pragma once

#include <husky/math/Vector3.hpp>
#include <husky/render/Component.hpp>

namespace husky {

class HUSKY_DLL QuadtreeTerrainNode
{
public:
  QuadtreeTerrainNode();
  ~QuadtreeTerrainNode();
  
  std::uint64_t id() const;
  
  unsigned char level; // [0,2^6-1]
  unsigned int x; // [0,2^29-1]
  unsigned int y; // [0,2^29-1]
};

class HUSKY_DLL PlanetComponent : public IComponent
{
public:
  PlanetComponent();
  virtual ~PlanetComponent();

  virtual void init(Entity *owner);
  virtual void draw(const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection) const;

  QuadtreeTerrainNode *root;
};

}
