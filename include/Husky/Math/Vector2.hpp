#pragma once

#include <cstdint>

namespace husky {

template<typename T>
class __declspec(dllexport) Vector2
{
public:
  union {
    T v[2];
    struct { T x, y; };
  };

  Vector2();
  Vector2(T xy);
  Vector2(T x, T y);

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
};

typedef Vector2<double> Vector2d;
typedef Vector2<float> Vector2f;
typedef Vector2<std::uint8_t> Vector2b;
typedef Vector2<std::int32_t> Vector2i;

}
