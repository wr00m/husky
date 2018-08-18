#include <husky/render/Viewport.hpp>

namespace husky {

Ray::Ray()
  : startPos()
  , dir()
{
}

Ray::Ray(const Vector3d &startPos, const Vector3d &dir)
  : startPos(startPos)
  , dir(dir)
{
}

void Ray::transform(const Matrix44d &mtx)
{
  startPos = (mtx * Vector4d(startPos, 1.0)).xyz;
  dir = (mtx * Vector4d(dir, 0.0)).xyz;
}

Ray operator*(const Matrix44d &mtx, const Ray &r)
{
  Ray ray = r;
  ray.transform(mtx);
  return ray;
}

Viewport::Viewport()
  : x(0), y(0), width(0), height(0)
{
}

Viewport::Viewport(int x, int y, int width, int height)
  : x(x), y(y), width(width), height(height)
{
}

void Viewport::set(int x, int y, int width, int height)
{
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
}

double Viewport::aspectRatio() const
{
  return (height != 0 ? (width / (double)height) : 0.0);
}

Vector3d Viewport::project(const Vector3d &worldPos, const Matrix44d &modelView, const Matrix44d &proj) const
{
  Vector4d v = proj * (modelView * Vector4d(worldPos, 1.0));
  v /= v.w;
  v = (v * 0.5 + 0.5);
  
  Vector3d windowPos;
  windowPos.x = (x + v.x * width);
  windowPos.y = (y + v.y * height);
  windowPos.z = v.z;
  
  return windowPos;
}

Vector3d Viewport::unproject(const Vector3d &windowPos, const Matrix44d &modelView, const Matrix44d &proj) const
{
  Matrix44d mvp = proj * modelView;
  Matrix44d mvpInv = mvp.inverted();
  return unproject(windowPos, mvpInv);
}

Vector3d Viewport::unproject(const Vector3d &windowPos, const Matrix44d &mvpInv) const
{
  Vector4d nd;
  nd.x = 2.0 * (windowPos.x - x) / width - 1.0;
  nd.y = 2.0 * (windowPos.y - y) / height - 1.0;
  nd.z = 2.0 * windowPos.z - 1.0;
  nd.w = 1.0;

  Vector4d worldPos = mvpInv * nd;
  worldPos /= worldPos.w;

  return worldPos.xyz;
}

Ray Viewport::getPickingRay(const Vector2d &windowPos, const Camera &cam) const
{
  const Matrix44d mvpInv = cam.viewInv * cam.projInv;

  Ray ray;
  ray.startPos = unproject({ windowPos, 0.0 }, mvpInv);

  if (cam.isOrtho()) {
    ray.dir = cam.forward();
  }
  else {
    ray.dir = (ray.startPos - cam.pos).normalized();
  }

  if (cam.isRevZ()) {
    ray.dir = -ray.dir;
  }

  return ray;
}

}
