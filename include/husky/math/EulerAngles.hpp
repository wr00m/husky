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

  union { // Radians
    struct { T yaw, pitch, roll; }; // These terms are useful for Tait-Bryan angles (not proper Euler angles)
    Vector3<T> angles;
  };

  EulerAngles(RotationOrder rotationOrder, T yaw, T pitch, T roll);
  EulerAngles(RotationOrder rotationOrder, const Quaternion<T> &q);
  EulerAngles(RotationOrder rotationOrder, const Matrix33<T> &m);
  EulerAngles(RotationOrder rotationOrder, const Matrix44<T> &m);

  template<typename T2>
  explicit EulerAngles(const EulerAngles<T2> &e) : EulerAngles(e.rotationOrder, T(e.yaw), T(e.pitch), T(e.roll)) {}

  template<typename T2>
  operator EulerAngles<T2>() const { return EulerAngles<T2>(rotationOrder, yaw, pitch, roll); }

  Quaternion<T> toQuaternion() const;
  Matrix33<T> toMatrix() const;
};

typedef EulerAngles<double> EulerAnglesd;
typedef EulerAngles<float> EulerAnglesf;

}
