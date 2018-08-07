#pragma once

#include <husky/math/Matrix44.hpp>

namespace husky {

class Entity;
class Viewport;

class HUSKY_DLL IComponent
{
public:
  IComponent();
  virtual ~IComponent();

  virtual void init(Entity *owner) = 0;
  virtual void draw(const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection) const = 0;

protected:
  Entity *owner;
};

class HUSKY_DLL DebugDrawComponent : public IComponent
{
public:
  DebugDrawComponent();
  virtual ~DebugDrawComponent();

  virtual void init(Entity *owner);
  virtual void draw(const Viewport &viewport, const Matrix44f &view, const Matrix44f &modelView, const Matrix44f &projection) const;
};

}
