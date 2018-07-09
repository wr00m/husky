#include <husky/render/Viewport.hpp>

namespace husky {

Viewport::Viewport()
  : x(0), y(0), width(0), height(0)
{
}

Viewport::Viewport(int x, int y, int width, int height)
  : x(x), y(y), width(width), height(height)
{
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
  Vector4d nd;
  nd.x = 2.0 * (windowPos.x - x) / width - 1.0;
  nd.y = 2.0 * (windowPos.y - y) / height - 1.0;
  nd.z = 2.0 * windowPos.z - 1.0;
  nd.w = 1.0;
  
  Matrix44d vp = proj * modelView;
  Matrix44d vpInv = vp.inverted();
  
  Vector4d worldPos = vpInv * nd;
  worldPos /= worldPos.w;
  
  return worldPos.xyz;
}

Vector3d Viewport::getPickingRayDir(const Vector2d &mousePos, const Camera &cam) const
{
  // TODO: Take viewport position and size into consideration
  Vector3d windowPos(mousePos.x, height - mousePos.y, 0.0); // Flip Y
  Vector3d worldPos = unproject(windowPos, cam.view, cam.projection);
  Vector3d dir = (worldPos - cam.position).normalized();
  return dir;
}

}
