#pragma once

#include <husky/math/Vector2.hpp>

namespace husky {

template<typename T>
class HUSKY_DLL Vector3
{
public:
  union {
    T val[3];
    Vector2<T> xy;
    struct { T x, y, z; };
    struct { T r, g, b; };
    struct { T s, t, p; };
    struct { T u, v; };
  };

  Vector3();
  explicit Vector3(T xyz);
  explicit Vector3(const T *xyz);
  Vector3(T x, T y, T z);
  Vector3(const Vector2<T> &xy, T z);

  template<typename T2>
  explicit Vector3(const Vector3<T2> &xyz) : x(T(xyz.x)), y(T(xyz.y)), z(T(xyz.z)) {}

  template<typename T2>
  operator Vector3<T2>() const { return Vector3<T2>(x, y, z); }

  void        set(T x, T y, T z);
  T           dot(const Vector3<T> &other) const;
  Vector3<T>  cross(const Vector3<T> &other) const;
  void        normalize();
  Vector3<T>  normalized() const;
  T           length() const;
  T           length2() const;
  T           angleAbs(const Vector3<T> &target) const;

  Vector3<T>  lerp(const Vector3<T> &target, T t) const;
  Vector3<T> nlerp(const Vector3<T> &target, T t) const;
  Vector3<T> slerp(const Vector3<T> &target, T t) const;

  Vector3<T>  operator- () const;
  Vector3<T>  operator+ (const Vector3<T> &other) const;
  Vector3<T>  operator- (const Vector3<T> &other) const;
  Vector3<T>  operator* (const Vector3<T> &other) const;
  Vector3<T>  operator/ (const Vector3<T> &other) const;
  Vector3<T>& operator+=(const Vector3<T> &other);
  Vector3<T>& operator-=(const Vector3<T> &other);
  Vector3<T>& operator*=(const Vector3<T> &other);
  Vector3<T>& operator/=(const Vector3<T> &other);
  Vector3<T>  operator+ (T t) const;
  Vector3<T>  operator- (T t) const;
  Vector3<T>  operator* (T t) const;
  Vector3<T>  operator/ (T t) const;
  Vector3<T>& operator+=(T t);
  Vector3<T>& operator-=(T t);
  Vector3<T>& operator*=(T t);
  Vector3<T>& operator/=(T t);

  T& operator[](int i) { return this->val[i]; }
  T  operator[](int i) const { return this->val[i]; }
};

typedef Vector3<double> Vector3d;
typedef Vector3<float> Vector3f;
typedef Vector3<std::uint8_t> Vector3b;
typedef Vector3<std::int32_t> Vector3i;

}
