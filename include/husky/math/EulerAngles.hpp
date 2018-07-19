#pragma once

#include <husky/math/Matrix44.hpp>
#include <husky/math/Quaternion.hpp>

namespace husky {

enum class RotationOrder { ZYX, ZYZ, ZXY, ZXZ, YXZ, YXY, YZX, YZY, XYZ, XYX, XZY, XZX };

template<typename T>
class HUSKY_DLL EulerAngles
{
public:
  RotationOrder rotationOrder;

  union {
    struct { T angle0Rad, angle1Rad, angle2Rad; };
    Vector3<T> anglesRad;
  };

  EulerAngles(RotationOrder rotationOrder, T angle0Rad, T angle1Rad, T angle2Rad);
  EulerAngles(RotationOrder rotationOrder, const Quaternion<T> &q);
  EulerAngles(RotationOrder rotationOrder, const Matrix33<T> &m);
  EulerAngles(RotationOrder rotationOrder, const Matrix44<T> &m);

  Quaternion<T> toQuaternion() const;
  Matrix33<T> toMatrix() const;
};

typedef EulerAngles<double> EulerAnglesd;
typedef EulerAngles<float> EulerAnglesf;

}
