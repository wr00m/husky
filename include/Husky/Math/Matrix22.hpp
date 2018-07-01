#pragma once

#include <Husky/Math/Vector2.hpp>

namespace husky {

template<typename T>
class __declspec(dllexport) Matrix22
{
public:
  union { // Column-major
    T m[4];
    Vector2<T> col[2];
    struct { T cr00, cr01, cr10, cr11; };
  };

  static Matrix22<T> identity();
  static Matrix22<T> diagonal(T t);
  static Matrix22<T> rotate(T rad);

  Matrix22();
  Matrix22(T cr00, T cr01, T cr10, T cr11);
  Matrix22(const Vector2<T> &col0, const Vector2<T> &col1);
  Matrix22(const T *m);

  template<typename T2>
  explicit Matrix22(const Matrix22<T2> &other) : col{ Vector2<T>(other.col[0]), Vector2<T>(other.col[1]) } {}

  template<typename T2>
  operator Matrix22<T2>() const { return Matrix22<T2>(cr00, cr01, cr10, cr11); }

  Vector2<T> row(int i) const;
  void transpose();
  void invert();
  Matrix22<T> transposed() const;
  Matrix22<T> inverted() const;

  Matrix22<T>& operator+=(const Matrix22<T> &other);
  Matrix22<T>& operator-=(const Matrix22<T> &other);
  Matrix22<T>& operator*=(const Matrix22<T> &other);
  Matrix22<T> operator+(const Matrix22<T> &other) const;
  Matrix22<T> operator-(const Matrix22<T> &other) const;
  Matrix22<T> operator*(const Matrix22<T> &other) const;
  Matrix22<T> operator-() const;
  Vector2<T> operator*(const Vector2<T> &v) const;
};

typedef Matrix22<double> Matrix22d;
typedef Matrix22<float> Matrix22f;

}
