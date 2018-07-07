#pragma once

#include <husky/math/Matrix33.hpp>

namespace husky {

template<typename T>
class __declspec(dllexport) Quaternion
{
public:
  static Quaternion<T> identity();
  static Quaternion<T> fromRotationMatrix(const Matrix33<T> &rotationMatrix);
  static Quaternion<T> fromDirections(const Vector3<T> &from, const Vector3<T> &to);
  static Quaternion<T> fromAxisAngle(T rad, Vector3<T> axis);
  //static Quaternion<T> fromEulerAngles(T yawRad, T pitchRad, T rollRad);
  
  union {
    struct { T x, y, z, w; };
    Vector3<T> xyz;
  };
  
  Quaternion();
  Quaternion(T xyzw);
  Quaternion(T x, T y, T z, T w);
  
  template<typename T2>
  explicit Quaternion(const Quaternion<T2> &xyzw) : x(T(xyzw.x)), y(T(xyzw.y)), z(T(xyzw.z)), w(T(xyzw.w)) {}
  
  template<typename T2>
  operator Quaternion<T2>() const { return Quaternion<T2>(x, y, z, w); }
  
  void          set(T x, T y, T z, T w);
  void          normalize();
  Quaternion<T> normalized() const;
  void          conjugate();
  Quaternion<T> conjugated() const;
  T             length() const;
  T             length2() const;
  T             dot(const Quaternion<T> &other) const;
  Matrix33<T>   toMatrix() const;
  T             toAxisAngle(Vector3<T> &axis) const;
  //Vector3<T> toEulerAngles() const;
  T             angleAbs(const Quaternion<T> &target) const;
  
  Quaternion<T> nlerp(const Quaternion<T> &target, T t) const;
  Quaternion<T> slerp(const Quaternion<T> &target, T t) const;
  
  Vector3<T> operator*(const Vector3<T> &v) const;
  Quaternion<T>& operator*=(const Quaternion<T> &other);
  Quaternion<T> operator*(const Quaternion<T> &other) const;
  Quaternion<T>& operator+=(T v);
  Quaternion<T>& operator-=(T v);
  Quaternion<T>& operator*=(T v);
  Quaternion<T>& operator/=(T v);
  Quaternion<T> operator+(T v) const;
  Quaternion<T> operator-(T v) const;
  Quaternion<T> operator*(T v) const;
  Quaternion<T> operator/(T v) const;
  Quaternion<T> operator-() const;
};

typedef Quaternion<double> Quaterniond;
typedef Quaternion<float> Quaternionf;

}
