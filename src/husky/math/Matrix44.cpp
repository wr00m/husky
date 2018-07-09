#include <husky/math/Matrix44.hpp>
#include <husky/math/Math.hpp>
#include <cmath>

// TODO: Also return analytical inverse in ortho, perspective*, lookAt, etc.?

namespace husky {

template<typename T>
Matrix44<T> Matrix44<T>::identity()
{
  return {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };
}

template<typename T>
Matrix44<T> Matrix44<T>::scale(const Vector3<T> &s)
{
  return {
    s.x,   0,   0, 0,
      0, s.y,   0, 0,
      0,   0, s.z, 0,
      0,   0,   0, 1
  };
}

template<typename T>
Matrix44<T> Matrix44<T>::translate(const Vector3<T> &pos)
{
  Matrix44<T> res = identity();
  res.col[3].xyz = pos;
  return res;
}

template<typename T>
Matrix44<T> Matrix44<T>::rotate(T rad, Vector3<T> axis)
{
  return { Matrix33<T>::rotate(rad,axis) };
}

template<typename T>
Matrix44<T> Matrix44<T>::ortho(T left, T right, T bottom, T top, T near, T far)
{
  Matrix44<T> res = identity();
  res.col[0].val[0] = T(2) / (right - left);
  res.col[1].val[1] = T(2) / (top - bottom);
  res.col[2].val[2] = -T(2) / (far - near);
  res.col[3].val[0] = -(right + left) / (right - left);
  res.col[3].val[1] = -(top + bottom) / (top - bottom);
  res.col[3].val[2] = -(far + near) / (far - near);
  return res;
}

template<typename T>
Matrix44<T> Matrix44<T>::perspective(T yFovRad, T aspectRatio, T near, T far)
{
  T top = near * std::tan(yFovRad * T(0.5));
  T right = top * aspectRatio;
  return frustum(-right, right, -top, top, near, far);
}

template<typename T>
Matrix44<T> Matrix44<T>::perspectiveInf(T yFovRad, T aspectRatio, T near, T epsilon) // 2.4e-7
{
  // Note: This differs slightly from http://www.terathon.com/gdc07_lengyel.pdf
  T e = T(1) / (std::tan(T(0.5) * yFovRad) * aspectRatio);

  Matrix44<T> m;
  m[0][0] = e;
  m[1][1] = e * aspectRatio;
  m[2][2] = epsilon - T(1);
  m[2][3] = T(-1);
  m[3][2] = (epsilon - T(2)) * near;
  m[3][3] = T(0);
  return m;
}

template<typename T>
Matrix44<T> Matrix44<T>::perspectiveInfRevZ(T yFovRad, T aspectRatio, T near)
{
  // https://nlguillemot.wordpress.com/2016/12/07/reversed-z-in-opengl/
  T e = T(1) / (std::tan(T(0.5) * yFovRad) * aspectRatio);

  Matrix44<T> m;
  m[0][0] = e;
  m[1][1] = e * aspectRatio;
  m[2][2] = T(-1);
  m[2][3] = T(0);
  m[3][2] = near;
  m[3][3] = T(0);
  return m;
}

template<typename T>
Matrix44<T> Matrix44<T>::frustum(T left, T right, T bottom, T top, T near, T far)
{
  T t1 = T(2) * near;
  T t2 = right - left;
  T t3 = top - bottom;
  T t4 = far - near;

  Matrix44<T> m;
  m[0][0] = t1 / t2;
  m[0][1] = 0;
  m[0][2] = 0;
  m[0][3] = 0;
  m[1][0] = 0;
  m[1][1] = t1 / t3;
  m[1][2] = 0;
  m[1][3] = 0;
  m[2][0] = (right + left) / t2;
  m[2][1] = (top + bottom) / t3;
  m[2][2] = (-far - near) / t4;
  m[2][3] = -1;
  m[3][0] = 0;
  m[3][1] = 0;
  m[3][2] = (-t1 * far) / t4;
  m[3][3] = 0;

  return m;
}

template<typename T>
Matrix44<T> Matrix44<T>::lookAt(const Vector3<T> &camPos, const Vector3<T> &lookAtPos, Vector3<T> upDir)
{
  upDir.normalize();

  Vector3<T> forwardDir = (lookAtPos - camPos).normalized();
  Vector3<T> rightDir = forwardDir.cross(upDir).normalized();
  upDir = rightDir.cross(forwardDir);

  Matrix44<T> m;
  m[0] = { rightDir, T(0) };
  m[1] = { upDir, T(0) };
  m[2] = { -forwardDir, T(0) };
  m[0][3] = -rightDir.dot(camPos);
  m[1][3] = -upDir.dot(camPos);
  m[2][3] = forwardDir.dot(camPos);
  m[3][3] = T(1);
  m.transpose();
  return m;
}

template<typename T>
Matrix44<T>::Matrix44()
  : m{} // Zero
{
}

template<typename T>
Matrix44<T>::Matrix44(const T *m)
  : m{ m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15] }
{
}

template<typename T>
Matrix44<T>::Matrix44(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33)
  : m{ m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33 }
{
}

template<typename T>
Matrix44<T>::Matrix44(const Vector4<T> &col0, const Vector4<T> &col1, const Vector4<T> &col2, const Vector4<T> &col3)
  : col{ col0, col1, col2, col3 }
{
}

template<typename T>
Matrix44<T>::Matrix44(const Matrix33<T> &other)
  : col{ { other.col[0], 0 }, { other.col[1], 0 }, { other.col[2], 0 }, { 0, 0, 0, 1 } }
{
}

template<typename T>
Vector4<T> Matrix44<T>::row(int i) const
{
  return Vector4<T>(col[0].val[i], col[1].val[i], col[2].val[i], col[3].val[i]);
}

template<typename T>
Matrix33<T> Matrix44<T>::get3x3() const
{
  return Matrix33<T>(col[0].xyz, col[1].xyz, col[2].xyz);
}

template<typename T>
void Matrix44<T>::transpose()
{
  *this = transposed();
}

template<typename T>
void Matrix44<T>::invert()
{
  Matrix44<T> tmp;
  tmp.m[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
  tmp.m[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
  tmp.m[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
  tmp.m[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
  tmp.m[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
  tmp.m[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
  tmp.m[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
  tmp.m[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
  tmp.m[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
  tmp.m[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
  tmp.m[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
  tmp.m[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
  tmp.m[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
  tmp.m[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
  tmp.m[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
  tmp.m[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

  T det = m[0] * tmp.m[0] + m[1] * tmp.m[4] + m[2] * tmp.m[8] + m[3] * tmp.m[12];

  if (det == 0) {
    *this = {}; // Set to 0
    return;
  }

  det = T(1) / det;

  for (int i = 0; i < 16; i++) {
    m[i] = tmp.m[i] * det;
  }
}

template<typename T>
Matrix44<T> Matrix44<T>::transposed() const
{
  return Matrix44<T>(row(0), row(1), row(2), row(3));
}

template<typename T>
Matrix44<T> Matrix44<T>::inverted() const
{
  Matrix44<T> res = *this;
  res.invert();
  return res;
}

template<typename T>
Matrix44<T>& Matrix44<T>::operator+=(const Matrix44<T> &other)
{
  *this = *this + other;
  return *this;
}

template<typename T>
Matrix44<T>& Matrix44<T>::operator-=(const Matrix44<T> &other)
{
  *this = *this - other;
  return *this;
}

template<typename T>
Matrix44<T>& Matrix44<T>::operator*=(const Matrix44<T> &other)
{
  *this = *this * other;
  return *this;
}

template<typename T>
Matrix44<T> Matrix44<T>::operator+(const Matrix44<T> &other) const
{
  return Matrix44<T>(col[0] + other.col[0], col[1] + other.col[1], col[2] + other.col[2], col[3] + other.col[3]);
}

template<typename T>
Matrix44<T> Matrix44<T>::operator-(const Matrix44<T> &other) const
{
  return Matrix44<T>(col[0] - other.col[0], col[1] - other.col[1], col[2] - other.col[2], col[3] - other.col[3]);
}

template<typename T>
Matrix44<T> Matrix44<T>::operator*(const Matrix44<T> &other) const
{
  Matrix44<T> m;
  m[0] = col[0] * other[0][0] + col[1] * other[0][1] + col[2] * other[0][2] + col[3] * other[0][3];
  m[1] = col[0] * other[1][0] + col[1] * other[1][1] + col[2] * other[1][2] + col[3] * other[1][3];
  m[2] = col[0] * other[2][0] + col[1] * other[2][1] + col[2] * other[2][2] + col[3] * other[2][3];
  m[3] = col[0] * other[3][0] + col[1] * other[3][1] + col[2] * other[3][2] + col[3] * other[3][3];
  return m;
}

template<typename T>
Matrix44<T> Matrix44<T>::operator-() const
{
  return Matrix44<T>(-col[0], -col[1], -col[2], -col[3]);
}

template<typename T>
Vector4<T> Matrix44<T>::operator*(const Vector4<T> &v) const
{
  Vector4<T> res;
  res[0] = row(0).dot(v);
  res[1] = row(1).dot(v);
  res[2] = row(2).dot(v);
  res[3] = row(3).dot(v);
  return res;
}

template class Matrix44<double>;
template class Matrix44<float>;

}
