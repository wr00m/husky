#include <Husky/Math/Matrix22.hpp>
#include <cmath>

namespace husky {

template<typename T>
Matrix22<T> Matrix22<T>::identity()
{
  return diagonal(1);
}

template<typename T>
Matrix22<T> Matrix22<T>::diagonal(T t)
{
  return Matrix22<T>(
    t, 0,
    0, t);
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
Matrix22<T>::Matrix22(T cr00, T cr01, T cr10, T cr11)
  : m{ cr00, cr01, cr10, cr11 }
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
  return identity(); // TODO
}

template<typename T>
Matrix22<T> Matrix22<T>::operator-() const
{
  return Matrix22<T>(-col[0], -col[1]);
}

template<typename T>
Vector2<T> Matrix22<T>::operator*(const Vector2<T> &v) const
{
  return{}; // TODO
}

template class Matrix22<double>;
template class Matrix22<float>;

}
