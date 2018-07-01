#include <Husky/Math/Matrix33.hpp>
#include <cmath>

namespace husky {

template<typename T>
Matrix33<T> Matrix33<T>::identity()
{
  return diagonal(1);
}

template<typename T>
Matrix33<T> Matrix33<T>::diagonal(T t)
{
  return Matrix33<T>(
    t, 0, 0,
    0, t, 0,
    0, 0, t);
}

template<typename T>
Matrix33<T> Matrix33<T>::rotate(T rad, const Vector3<T> &axis)
{
  return identity(); // TODO
}

template<typename T>
Matrix33<T> Matrix33<T>::rotateX(T rad)
{
  return rotate(rad, { 1, 0, 0 });
}

template<typename T>
Matrix33<T> Matrix33<T>::rotateY(T rad)
{
  return rotate(rad, { 0, 1, 0 });
}

template<typename T>
Matrix33<T> Matrix33<T>::rotateZ(T rad)
{
  return rotate(rad, { 0, 0, 1 });
}

template<typename T>
Matrix33<T>::Matrix33()
  : m{} // Zero
{
}

template<typename T>
Matrix33<T>::Matrix33(const T *m)
  : m{ m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8] }
{
}

template<typename T>
Matrix33<T>::Matrix33(T cr00, T cr01, T cr02, T cr10, T cr11, T cr12, T cr20, T cr21, T cr22)
  : m{ cr00, cr01, cr02, cr10, cr11, cr12, cr20, cr21, cr22 }
{
}

template<typename T>
Matrix33<T>::Matrix33(const Vector3<T> &col0, const Vector3<T> &col1, const Vector3<T> &col2)
  : col{ col0, col1, col2 }
{
}

template<typename T>
Vector3<T> Matrix33<T>::row(int i) const
{
  return Vector3<T>(col[0].val[i], col[1].val[i], col[2].val[i]);
}

template<typename T>
void Matrix33<T>::transpose()
{
  *this = transposed();
}

template<typename T>
void Matrix33<T>::invert()
{
  *this = {}; // TODO
}

template<typename T>
Matrix33<T> Matrix33<T>::transposed() const
{
  return Matrix33<T>(row(0), row(1), row(2));
}

template<typename T>
Matrix33<T> Matrix33<T>::inverted() const
{
  Matrix33<T> res = *this;
  res.invert();
  return res;
}

template<typename T>
Matrix33<T>& Matrix33<T>::operator+=(const Matrix33<T> &other)
{
  *this = *this + other;
  return *this;
}

template<typename T>
Matrix33<T>& Matrix33<T>::operator-=(const Matrix33<T> &other)
{
  *this = *this - other;
  return *this;
}

template<typename T>
Matrix33<T>& Matrix33<T>::operator*=(const Matrix33<T> &other)
{
  *this = *this * other;
  return *this;
}

template<typename T>
Matrix33<T> Matrix33<T>::operator+(const Matrix33<T> &other) const
{
  return Matrix33<T>(col[0] + other.col[0], col[1] + other.col[1], col[2] + other.col[2]);
}

template<typename T>
Matrix33<T> Matrix33<T>::operator-(const Matrix33<T> &other) const
{
  return Matrix33<T>(col[0] - other.col[0], col[1] - other.col[1], col[2] - other.col[2]);
}

template<typename T>
Matrix33<T> Matrix33<T>::operator*(const Matrix33<T> &other) const
{
  return identity(); // TODO
}

template<typename T>
Matrix33<T> Matrix33<T>::operator-() const
{
  return Matrix33<T>(-col[0], -col[1], -col[2]);
}

template<typename T>
Vector3<T> Matrix33<T>::operator*(const Vector3<T> &v) const
{
  return{}; // TODO
}

template class Matrix33<double>;
template class Matrix33<float>;

}
