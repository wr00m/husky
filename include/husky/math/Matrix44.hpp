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
    struct { T m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33; };
  };

  static Matrix44<T> identity();
  static Matrix44<T> scale(const Vector3<T> &s);
  static Matrix44<T> translate(const Vector3<T> &pos);
  static Matrix44<T> rotate(T rad, Vector3<T> axis);
  static Matrix44<T> ortho(T left, T right, T bottom, T top, T near, T far);
  static Matrix44<T> perspective(T yFovRad, T aspectRatio, T near, T far);
  static Matrix44<T> perspectiveInf(T yFovRad, T aspectRatio, T near, T epsilon = T(0));
  static Matrix44<T> perspectiveInfRevZ(T yFovRad, T aspectRatio, T near);
  static Matrix44<T> frustum(T left, T right, T bottom, T top, T near, T far);
  static Matrix44<T> lookAt(const Vector3<T> &camPos, const Vector3<T> &lookAtPos, Vector3<T> upDir);

  Matrix44();
  Matrix44(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33);
  Matrix44(const Vector4<T> &col0, const Vector4<T> &col1, const Vector4<T> &col2, const Vector4<T> &col3);
  Matrix44(const T *m);
  Matrix44(const Matrix33<T> &other);

  template<typename T2>
  explicit Matrix44(const Matrix44<T2> &other) : col{ Vector4<T>(other.col[0]), Vector4<T>(other.col[1]), Vector4<T>(other.col[2]), Vector4<T>(other.col[3]) } {}

  template<typename T2>
  operator Matrix44<T2>() const { return Matrix44<T2>(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33); }

  Vector4<T>  row(int i) const;
  Matrix33<T> get3x3() const;
  void        transpose();
  Matrix44<T> transposed() const;
  void        invert();
  Matrix44<T> inverted() const;

  Vector4<T> operator*(const Vector4<T> &v) const;

  Matrix44<T>  operator- () const;
  Matrix44<T>  operator+ (const Matrix44<T> &other) const;
  Matrix44<T>& operator+=(const Matrix44<T> &other);
  Matrix44<T>  operator- (const Matrix44<T> &other) const;
  Matrix44<T>& operator-=(const Matrix44<T> &other);
  Matrix44<T>  operator* (const Matrix44<T> &other) const;
  Matrix44<T>& operator*=(const Matrix44<T> &other);

  Vector4<T>& operator[](int colIndex) { return this->col[colIndex]; }
  const Vector4<T>& operator[](int colIndex) const { return this->col[colIndex]; }
};

typedef Matrix44<double> Matrix44d;
typedef Matrix44<float> Matrix44f;

}
