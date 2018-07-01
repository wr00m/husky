#pragma once

#include <Husky/Math/Vector2.hpp>

namespace husky {

template<typename T>
class __declspec(dllexport) Vector3
{
public:
  union {
    T val[3];
    struct { T x, y, z; };
    struct { T r, g, b; };
    struct { T s, t, p; };
    struct { T u, v; };
    struct { T yaw, pitch, roll; };
  };

  Vector3();
  Vector3(T xyz);
  Vector3(T x, T y, T z);
  Vector3(const Vector2<T> &xy, T z);

  template<typename T2>
  explicit Vector3(const Vector3<T2> &xyz) : x(T(xyz.x)), y(T(xyz.y)), z(T(xyz.z)) {}

  template<typename T2>
  operator Vector3<T2>() const { return Vector3<T2>(x, y, z); }

  void set(T x, T y, T z);
  void normalize();
  T length() const;
  T length2() const;
  T dot(const Vector3<T> &other) const;
  Vector3<T> cross(const Vector3<T> &other) const;
  Vector3<T> normalized() const;
  Vector2<T> xy() const;

  Vector3<T>& operator+=(const Vector3<T> &other);
  Vector3<T>& operator-=(const Vector3<T> &other);
  Vector3<T>& operator*=(const Vector3<T> &other);
  Vector3<T>& operator/=(const Vector3<T> &other);
  Vector3<T> operator+(const Vector3<T> &other) const;
  Vector3<T> operator-(const Vector3<T> &other) const;
  Vector3<T> operator*(const Vector3<T> &other) const;
  Vector3<T> operator/(const Vector3<T> &other) const;
  Vector3<T> operator-() const;
};

typedef Vector3<double> Vector3d;
typedef Vector3<float> Vector3f;
typedef Vector3<std::uint8_t> Vector3b;
typedef Vector3<std::int32_t> Vector3i;

}
