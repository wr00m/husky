#pragma once

#include <husky/math/Vector2.hpp>

namespace husky {

template<typename T>
class HUSKY_DLL Matrix22
{
public:
  union { // Column-major
    T m[4];
    Vector2<T> col[2];
    struct { T m00, m01, m10, m11; };
  };

  static Matrix22<T> identity();
  static Matrix22<T> scale(const Vector2<T> &s);
  static Matrix22<T> rotate(T rad);

  Matrix22();
  Matrix22(T m00, T m01, T m10, T m11);
  Matrix22(const Vector2<T> &col0, const Vector2<T> &col1);
  Matrix22(const T *m);

  template<typename T2>
  explicit Matrix22(const Matrix22<T2> &other) : col{ Vector2<T>(other.col[0]), Vector2<T>(other.col[1]) } {}

  template<typename T2>
  operator Matrix22<T2>() const { return Matrix22<T2>(m00, m01, m10, m11); }

  Vector2<T>  row(int i) const;
  void        transpose();
  Matrix22<T> transposed() const;
  void        invert();
  Matrix22<T> inverted() const;
  T           determinant() const;

  Vector2<T> operator*(const Vector2<T> &v) const;

  Matrix22<T>  operator- () const;
  Matrix22<T>  operator+ (const Matrix22<T> &other) const;
  Matrix22<T>& operator+=(const Matrix22<T> &other);
  Matrix22<T>  operator- (const Matrix22<T> &other) const;
  Matrix22<T>& operator-=(const Matrix22<T> &other);
  Matrix22<T>  operator* (const Matrix22<T> &other) const;
  Matrix22<T>& operator*=(const Matrix22<T> &other);

  Vector2<T>& operator[](int colIndex) { return this->col[colIndex]; }
  const Vector2<T>& operator[](int colIndex) const { return this->col[colIndex]; }
};

typedef Matrix22<double> Matrix22d;
typedef Matrix22<float> Matrix22f;

}
