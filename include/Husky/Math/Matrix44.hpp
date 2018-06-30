#pragma once

#include <Husky/Math/Vector4.hpp>
#include <Husky/Math/Matrix33.hpp>

namespace husky {

template<typename T>
class __declspec(dllexport) Matrix44
{
public:
  union { // Column-major
    T m[16];
    Vector4<T> col[4];
    struct { T cr00, cr01, cr02, cr03, cr10, cr11, cr12, cr13, cr20, cr21, cr22, cr23, cr30, cr31, cr32, cr33; };
  };

  static Matrix44<T> identity();
  static Matrix44<T> diagonal(T t);
  static Matrix44<T> translate(const Vector3<T> &pos);
  static Matrix44<T> rotate(T rad, const Vector3<T> &axis);
  static Matrix44<T> rotateX(T rad);
  static Matrix44<T> rotateY(T rad);
  static Matrix44<T> rotateZ(T rad);

  Matrix44();
  Matrix44(T cr00, T cr01, T cr02, T cr03, T cr10, T cr11, T cr12, T cr13, T cr20, T cr21, T cr22, T cr23, T cr30, T cr31, T cr32, T cr33);
  Matrix44(const Vector4<T> &col0, const Vector4<T> &col1, const Vector4<T> &col2, const Vector4<T> &col3);
  Matrix44(const T *m);

  void transpose();
  void invert();
  Matrix44<T> transposed() const;
  Matrix44<T> inverted() const;

  Matrix44<T>& operator+=(const Matrix44<T> &other);
  Matrix44<T>& operator-=(const Matrix44<T> &other);
  Matrix44<T>& operator*=(const Matrix44<T> &other);
  Matrix44<T> operator+(const Matrix44<T> &other) const;
  Matrix44<T> operator-(const Matrix44<T> &other) const;
  Matrix44<T> operator*(const Matrix44<T> &other) const;
  Matrix44<T> operator-() const;
};

typedef Matrix44<double> Matrix44d;
typedef Matrix44<float> Matrix44f;

}
