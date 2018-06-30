#pragma once

#include <Husky/Math/Vector3.hpp>

namespace husky {

template<typename T>
class __declspec(dllexport) Vector4
{
public:
  union {
    T v[4];
    struct { T x, y, z, w; };
    struct { T r, g, b, a; };
  };

  Vector4();
  Vector4(T xyzw);
  Vector4(T x, T y, T z, T w);
  Vector4(const Vector2<T> &xy, T z, T w);
  Vector4(const Vector3<T> &xyz, T w);

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
};

typedef Vector4<double> Vector4d;
typedef Vector4<float> Vector4f;
typedef Vector4<std::uint8_t> Vector4b;
typedef Vector4<std::int32_t> Vector4i;

}
