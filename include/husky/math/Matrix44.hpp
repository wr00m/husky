#pragma once

#include <husky/math/Vector4.hpp>
#include <husky/math/Matrix33.hpp>

namespace husky {

template<typename T>
class __declspec(dllexport) Matrix44
{
public:
  union { // Column-major
    T m[16];
    Vector4<T> col[4];
    //struct { Vector4<T> col0, col1, col2, col3; };
    struct { T cr00, cr01, cr02, cr03, cr10, cr11, cr12, cr13, cr20, cr21, cr22, cr23, cr30, cr31, cr32, cr33; };
  };

  static Matrix44<T> identity();
  static Matrix44<T> scale(const Vector3<T> &s);
  static Matrix44<T> translate(const Vector3<T> &pos);
  static Matrix44<T> rotate(T rad, Vector3<T> axis);
  static Matrix44<T> ortho(T left, T right, T bottom, T top, T near, T far);
  static Matrix44<T> perspective(T fovY, T aspectRatio, T near, T far);
  static Matrix44<T> frustum(T left, T right, T bottom, T top, T near, T far);
  static Matrix44<T> lookAt(const Vector3<T> &camPos, const Vector3<T> &lookAtPos, Vector3<T> upDir);

  Matrix44();
  Matrix44(T cr00, T cr01, T cr02, T cr03, T cr10, T cr11, T cr12, T cr13, T cr20, T cr21, T cr22, T cr23, T cr30, T cr31, T cr32, T cr33);
  Matrix44(const Vector4<T> &col0, const Vector4<T> &col1, const Vector4<T> &col2, const Vector4<T> &col3);
  Matrix44(const T *m);
  Matrix44(const Matrix33<T> &other);

  template<typename T2>
  explicit Matrix44(const Matrix44<T2> &other) : col{ Vector4<T>(other.col[0]), Vector4<T>(other.col[1]), Vector4<T>(other.col[2]), Vector4<T>(other.col[3]) } {}

  template<typename T2>
  operator Matrix44<T2>() const { return Matrix44<T2>(cr00, cr01, cr02, cr03, cr10, cr11, cr12, cr13, cr20, cr21, cr22, cr23, cr30, cr31, cr32, cr33); }

  Vector4<T> row(int i) const;
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
  Vector4<T> operator*(const Vector4<T> &v) const;
  Vector3<T> operator*(const Vector3<T> &v) const;
  inline Vector4<T>& operator[](int colIndex) { return this->col[colIndex]; }
  inline const Vector4<T>& operator[](int colIndex) const { return this->col[colIndex]; }
};

typedef Matrix44<double> Matrix44d;
typedef Matrix44<float> Matrix44f;

}
