#pragma once

#include <husky/math/EulerAngles.hpp>
#include <husky/math/Matrix44.hpp>
#include <husky/math/Quaternion.hpp>

namespace husky {

template<typename T>
class HUSKY_DLL Transform
{
public:
  Transform();
  Transform(const Vector3<T> &translation, const EulerAngles<T> &eulerAngles, const Vector3<T> &scale);
  Transform(const Matrix44<T> &mtx);
  Transform(const Matrix44<T> &mtx, const Matrix44<T> &mtxInv);

  Vector3<T> translation;
  EulerAngles<T> eulerAngles;
  Vector3<T> scale;
  Matrix44<T> mtx;
  Matrix44<T> mtxInv;
};

typedef Transform<double> Transformd;
typedef Transform<float> Transformf;

}
