#include <Husky/Math/Matrix44.hpp>
#include <cmath>

namespace husky {

template<typename T>
Matrix44<T> Matrix44<T>::identity()
{
  return diagonal(1);
}

template<typename T>
Matrix44<T> Matrix44<T>::diagonal(T t)
{
  return Matrix44<T>(
    t, 0, 0, 0,
    0, t, 0, 0,
    0, 0, t, 0,
    0, 0, 0, t);
}

template<typename T>
Matrix44<T> Matrix44<T>::translate(const Vector3<T> &pos)
{
  Matrix44<T> res = identity();
  res.col[3] = { pos, 0 };
  return res;
}

template<typename T>
Matrix44<T> Matrix44<T>::rotate(T rad, const Vector3<T> &axis)
{
  return identity(); // TODO
}

template<typename T>
Matrix44<T> Matrix44<T>::rotateX(T rad)
{
  return rotate(rad, { 1, 0, 0 });
}

template<typename T>
Matrix44<T> Matrix44<T>::rotateY(T rad)
{
  return rotate(rad, { 0, 1, 0 });
}

template<typename T>
Matrix44<T> Matrix44<T>::rotateZ(T rad)
{
  return rotate(rad, { 0, 0, 1 });
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
Matrix44<T>::Matrix44(T cr00, T cr01, T cr02, T cr03, T cr10, T cr11, T cr12, T cr13, T cr20, T cr21, T cr22, T cr23, T cr30, T cr31, T cr32, T cr33)
  : m{ cr00, cr01, cr02, cr03, cr10, cr11, cr12, cr13, cr20, cr21, cr22, cr23, cr30, cr31, cr32, cr33 }
{
}

template<typename T>
Matrix44<T>::Matrix44(const Vector4<T> &col0, const Vector4<T> &col1, const Vector4<T> &col2, const Vector4<T> &col3)
  : col{ col0, col1, col2, col3 }
{
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
  return Matrix44<T>(
    cr00, cr10, cr20, cr30,
    cr01, cr11, cr21, cr31,
    cr02, cr12, cr22, cr32,
    cr03, cr13, cr23, cr33);
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
  return identity(); // TODO
}

template<typename T>
Matrix44<T> Matrix44<T>::operator-() const
{
  return Matrix44<T>(-col[0], -col[1], -col[2], -col[3]);
}

template class Matrix44<double>;
template class Matrix44<float>;

}
