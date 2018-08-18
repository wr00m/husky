#pragma once

#include <husky/Common.hpp>

namespace husky {

template<typename T>
class HUSKY_DLL Vector2
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
  explicit Vector2(T xy);
  explicit Vector2(const T *xy);
  Vector2(T x, T y);

  template<typename T2>
  explicit Vector2(const Vector2<T2> &xy) : x(T(xy.x)), y(T(xy.y)) {}

  template<typename T2>
  operator Vector2<T2>() const { return Vector2<T2>(x, y); }

  void        set(T x, T y);
  T           dot(const Vector2<T> &other) const;
  void        normalize();
  Vector2<T>  normalized() const;
  T           length() const;
  T           length2() const;
  T           angleSigned(const Vector2<T> &target) const;
  T           min() const;
  T           max() const;

  Vector2<T>  lerp(const Vector2<T> &target, T t) const;
  Vector2<T> nlerp(const Vector2<T> &target, T t) const;

  Vector2<T>  operator- () const;
  Vector2<T>  operator+ (const Vector2<T> &other) const;
  Vector2<T>  operator- (const Vector2<T> &other) const;
  Vector2<T>  operator* (const Vector2<T> &other) const;
  Vector2<T>  operator/ (const Vector2<T> &other) const;
  Vector2<T>& operator+=(const Vector2<T> &other);
  Vector2<T>& operator-=(const Vector2<T> &other);
  Vector2<T>& operator*=(const Vector2<T> &other);
  Vector2<T>& operator/=(const Vector2<T> &other);
  Vector2<T>  operator+ (T t) const;
  Vector2<T>  operator- (T t) const;
  Vector2<T>  operator* (T t) const;
  Vector2<T>  operator/ (T t) const;
  Vector2<T>& operator+=(T t);
  Vector2<T>& operator-=(T t);
  Vector2<T>& operator*=(T t);
  Vector2<T>& operator/=(T t);

  T& operator[](int i) { return this->val[i]; }
  T  operator[](int i) const { return this->val[i]; }
};

typedef Vector2<double> Vector2d;
typedef Vector2<float> Vector2f;
typedef Vector2<std::uint8_t> Vector2b;
typedef Vector2<std::int32_t> Vector2i;

}
