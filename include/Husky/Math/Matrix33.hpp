#pragma once

#include <Husky/Math/Vector3.hpp>

namespace husky {

template<typename T>
class __declspec(dllexport) Matrix33
{
public:
  union { // Column-major
    T m[9];
    Vector3<T> col[3];
    struct { T cr00, cr01, cr02, cr10, cr11, cr12, cr20, cr21, cr22; };
  };

  static Matrix33<T> identity();
  static Matrix33<T> diagonal(T t);
  static Matrix33<T> rotate(T rad, const Vector3<T> &axis);
  static Matrix33<T> rotateX(T rad);
  static Matrix33<T> rotateY(T rad);
  static Matrix33<T> rotateZ(T rad);

  Matrix33();
  Matrix33(T cr00, T cr01, T cr02, T cr10, T cr11, T cr12, T cr20, T cr21, T cr22);
  Matrix33(const Vector3<T> &col0, const Vector3<T> &col1, const Vector3<T> &col2);
  Matrix33(const T *m);

  template<typename T2>
  explicit Matrix33(const Matrix33<T2> &other) : col{ Vector3<T>(other.col[0]), Vector3<T>(other.col[1]), Vector3<T>(other.col[2]) } {}

  template<typename T2>
  operator Matrix33<T2>() const { return Matrix33<T2>(cr00, cr01, cr02, cr10, cr11, cr12, cr20, cr21, cr22); }

  Vector3<T> row(int i) const;
  void transpose();
  void invert();
  Matrix33<T> transposed() const;
  Matrix33<T> inverted() const;

  Matrix33<T>& operator+=(const Matrix33<T> &other);
  Matrix33<T>& operator-=(const Matrix33<T> &other);
  Matrix33<T>& operator*=(const Matrix33<T> &other);
  Matrix33<T> operator+(const Matrix33<T> &other) const;
  Matrix33<T> operator-(const Matrix33<T> &other) const;
  Matrix33<T> operator*(const Matrix33<T> &other) const;
  Matrix33<T> operator-() const;
  Vector3<T> operator*(const Vector3<T> &v) const;
};

typedef Matrix33<double> Matrix33d;
typedef Matrix33<float> Matrix33f;

}
