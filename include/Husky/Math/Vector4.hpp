#pragma once

#include <husky/math/Vector3.hpp>

namespace husky {

template<typename T>
class __declspec(dllexport) Vector4
{
public:
  union {
    T val[4];
    struct { T x, y, z, w; };
    struct { T r, g, b, a; };
    struct { T s, t, p, q; };
    struct { T u, v; };
  };

  Vector4();
  Vector4(T xyzw);
  Vector4(T x, T y, T z, T w);
  Vector4(const Vector2<T> &xy, T z, T w);
  Vector4(const Vector3<T> &xyz, T w);

  template<typename T2>
  explicit Vector4(const Vector4<T2> &xyzw) : x(T(xyzw.x)), y(T(xyzw.y)), z(T(xyzw.z)), w(T(xyzw.w)) {}

  template<typename T2>
  operator Vector4<T2>() const { return Vector4<T2>(x, y, z, w); }

  void set(T x, T y, T z, T w);
  void normalize();
  T length() const;
  T length2() const;
  T dot(const Vector4<T> &other) const;
  Vector4<T> normalized() const;
  Vector2<T> xy() const;
  Vector3<T> xyz() const;

  Vector4<T>& operator+=(const Vector4<T> &other);
  Vector4<T>& operator-=(const Vector4<T> &other);
  Vector4<T>& operator*=(const Vector4<T> &other);
  Vector4<T>& operator/=(const Vector4<T> &other);
  Vector4<T> operator+(const Vector4<T> &other) const;
  Vector4<T> operator-(const Vector4<T> &other) const;
  Vector4<T> operator*(const Vector4<T> &other) const;
  Vector4<T> operator/(const Vector4<T> &other) const;
  Vector4<T> operator-() const;
  inline T& operator[](int i) { return this->val[i]; }
  inline T operator[](int i) const { return this->val[i]; }
};

typedef Vector4<double> Vector4d;
typedef Vector4<float> Vector4f;
typedef Vector4<std::uint8_t> Vector4b;
typedef Vector4<std::int32_t> Vector4i;

}
