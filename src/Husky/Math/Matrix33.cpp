#include <husky/math/Matrix33.hpp>
#include <cmath>

namespace husky {

template<typename T>
Matrix33<T> Matrix33<T>::identity()
{
  return {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1
  };
}

template<typename T>
Matrix33<T> Matrix33<T>::scale(const Vector3<T> &s)
{
  return {
    s.x,   0,   0,
      0, s.y,   0,
      0,   0, s.z
  };
}

template<typename T>
Matrix33<T> Matrix33<T>::rotate(T rad, Vector3<T> axis)
{
  axis.normalize();

  T c = std::cos(rad);
  T s = std::sin(rad);
  T t = 1 - c;

  Matrix33<T> res;
  res[0][0] = t * axis.x * axis.x + c;
  res[0][1] = t * axis.x * axis.y + axis.z * s;
  res[0][2] = t * axis.x * axis.z - axis.y * s;
  res[1][0] = t * axis.x * axis.y - axis.z * s;
  res[1][1] = t * axis.y * axis.y + c;
  res[1][2] = t * axis.y * axis.z + axis.x * s;
  res[2][0] = t * axis.x * axis.z + axis.y * s;
  res[2][1] = t * axis.y * axis.z - axis.x * s;
  res[2][2] = t * axis.z * axis.z + c;
  return res;
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
Matrix33<T>::Matrix33(const Matrix22<T> &other)
  : col{ { other.col[0], 0 }, { other.col[1], 0 }, { 0, 0, 1 } }
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
