#pragma once

namespace husky {

template<typename T>
class __declspec(dllexport) Viewport
{
public:
  int x, y, width, height;

  Viewport() : x(0), y(0), width(0), height(0) {}
  Viewport(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}
};

}
