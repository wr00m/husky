#pragma once

#include <cstdint>

namespace husky {

template<typename T>
class __declspec(dllexport) Vector2
{
public:
  union {
    T val[2];
    struct { T x, y; };
    struct { T r, g; };
    struct { T s, t; };
    struct { T u, v; };
  };

  Vector2();
  Vector2(T xy);
  Vector2(T x, T y);

  template<typename T2>
  explicit Vector2(const Vector2<T2> &xy) : x(T(xy.x)), y(T(xy.y)) {}

  template<typename T2>
  operator Vector2<T2>() const { return Vector2<T2>(x, y); }

  void set(T x, T y);
  void normalize();
  T length() const;
  T length2() const;
  T dot(const Vector2<T> &other) const;
  Vector2<T> normalized() const;

  Vector2<T>& operator+=(const Vector2<T> &other);
  Vector2<T>& operator-=(const Vector2<T> &other);
  Vector2<T>& operator*=(const Vector2<T> &other);
  Vector2<T>& operator/=(const Vector2<T> &other);
  Vector2<T> operator+(const Vector2<T> &other) const;
  Vector2<T> operator-(const Vector2<T> &other) const;
  Vector2<T> operator*(const Vector2<T> &other) const;
  Vector2<T> operator/(const Vector2<T> &other) const;
  Vector2<T> operator-() const;
  inline T& operator[](int i) { return this->val[i]; }
  inline T operator[](int i) const { return this->val[i]; }
};

typedef Vector2<double> Vector2d;
typedef Vector2<float> Vector2f;
typedef Vector2<std::uint8_t> Vector2b;
typedef Vector2<std::int32_t> Vector2i;

}
