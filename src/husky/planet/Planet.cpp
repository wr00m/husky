#include <husky/planet/Planet.hpp>

namespace husky {

QuadtreeTerrainNode::QuadtreeTerrainNode()
{
}

QuadtreeTerrainNode::~QuadtreeTerrainNode()
{
}

std::uint64_t QuadtreeTerrainNode::id() const
{
  //return (level << 58) | (x << 29) | (y << 0);
  return 0; // TODO
}

PlanetComponent::PlanetComponent()
{
}

PlanetComponent::~PlanetComponent()
{
}

void PlanetComponent::init(Entity *owner)
{
}

void PlanetComponent::draw(const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection) const
{
}

}
