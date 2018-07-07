#include <husky/math/Matrix22.hpp>
#include <cmath>

namespace husky {

template<typename T>
Matrix22<T> Matrix22<T>::identity()
{
  return {
    1, 0,
    0, 1
  };
}

template<typename T>
Matrix22<T> Matrix22<T>::scale(const Vector2<T> &s)
{
  return {
    s.x, 0,
    0, s.y
  };
}

template<typename T>
Matrix22<T> Matrix22<T>::rotate(T rad)
{
  return identity(); // TODO
}

template<typename T>
Matrix22<T>::Matrix22()
  : m{} // Zero
{
}

template<typename T>
Matrix22<T>::Matrix22(const T *m)
  : m{ m[0], m[1], m[2], m[3] }
{
}

template<typename T>
Matrix22<T>::Matrix22(T m00, T m01, T m10, T m11)
  : m{ m00, m01, m10, m11 }
{
}

template<typename T>
Matrix22<T>::Matrix22(const Vector2<T> &col0, const Vector2<T> &col1)
  : col{ col0, col1 }
{
}

template<typename T>
Vector2<T> Matrix22<T>::row(int i) const
{
  return Vector2<T>(col[0].val[i], col[1].val[i]);
}

template<typename T>
void Matrix22<T>::transpose()
{
  *this = transposed();
}

template<typename T>
void Matrix22<T>::invert()
{
  *this = {}; // TODO
}

template<typename T>
Matrix22<T> Matrix22<T>::transposed() const
{
  return Matrix22<T>(row(0), row(1));
}

template<typename T>
Matrix22<T> Matrix22<T>::inverted() const
{
  Matrix22<T> res = *this;
  res.invert();
  return res;
}

template<typename T>
T Matrix22<T>::determinant() const
{
  return col[0][0] * col[1][1] - col[1][0] * col[0][1];
}

template<typename T>
Matrix22<T>& Matrix22<T>::operator+=(const Matrix22<T> &other)
{
  *this = *this + other;
  return *this;
}

template<typename T>
Matrix22<T>& Matrix22<T>::operator-=(const Matrix22<T> &other)
{
  *this = *this - other;
  return *this;
}

template<typename T>
Matrix22<T>& Matrix22<T>::operator*=(const Matrix22<T> &other)
{
  *this = *this * other;
  return *this;
}

template<typename T>
Matrix22<T> Matrix22<T>::operator+(const Matrix22<T> &other) const
{
  return Matrix22<T>(col[0] + other.col[0], col[1] + other.col[1]);
}

template<typename T>
Matrix22<T> Matrix22<T>::operator-(const Matrix22<T> &other) const
{
  return Matrix22<T>(col[0] - other.col[0], col[1] - other.col[1]);
}

template<typename T>
Matrix22<T> Matrix22<T>::operator*(const Matrix22<T> &other) const
{
  Matrix22<T> m;
  m[0] = col[0] * other[0][0] + col[1] * other[0][1];
  m[1] = col[0] * other[1][0] + col[1] * other[1][1];
  return m;
}

template<typename T>
Matrix22<T> Matrix22<T>::operator-() const
{
  return Matrix22<T>(-col[0], -col[1]);
}

template<typename T>
Vector2<T> Matrix22<T>::operator*(const Vector2<T> &v) const
{
  Vector2<T> res;
  res[0] = row(0).dot(v);
  res[1] = row(1).dot(v);
  return v;
}

template class Matrix22<double>;
template class Matrix22<float>;

}
