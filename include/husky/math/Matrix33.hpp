#pragma once

#include <husky/math/Vector3.hpp>
#include <husky/math/Matrix22.hpp>

namespace husky {

template<typename T>
class HUSKY_DLL Matrix33
{
public:
  union { // Column-major
    T m[9];
    Vector3<T> col[3];
    struct { T m00, m01, m02, m10, m11, m12, m20, m21, m22; };
  };

  static Matrix33<T> identity();
  static Matrix33<T> scale(const Vector3<T> &s);
  static Matrix33<T> rotate(T rad, Vector3<T> axis);

  Matrix33();
  Matrix33(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22);
  Matrix33(const Vector3<T> &col0, const Vector3<T> &col1, const Vector3<T> &col2);
  Matrix33(const T *m);
  Matrix33(const Matrix22<T> &other);

  template<typename T2>
  explicit Matrix33(const Matrix33<T2> &other) : col{ Vector3<T>(other.col[0]), Vector3<T>(other.col[1]), Vector3<T>(other.col[2]) } {}

  template<typename T2>
  operator Matrix33<T2>() const { return Matrix33<T2>(m00, m01, m02, m10, m11, m12, m20, m21, m22); }

  Vector3<T>  row(int i) const;
  void        transpose();
  Matrix33<T> transposed() const;
  void        invert();
  Matrix33<T> inverted() const;
  T           determinant() const;

  Vector3<T> operator*(const Vector3<T> &v) const;

  Matrix33<T>  operator- () const;
  Matrix33<T>  operator+ (const Matrix33<T> &other) const;
  Matrix33<T>& operator+=(const Matrix33<T> &other);
  Matrix33<T>  operator- (const Matrix33<T> &other) const;
  Matrix33<T>& operator-=(const Matrix33<T> &other);
  Matrix33<T>  operator* (const Matrix33<T> &other) const;
  Matrix33<T>& operator*=(const Matrix33<T> &other);

  Vector3<T>& operator[](int colIndex) { return this->col[colIndex]; }
  const Vector3<T>& operator[](int colIndex) const { return this->col[colIndex]; }
};

typedef Matrix33<double> Matrix33d;
typedef Matrix33<float> Matrix33f;

}
