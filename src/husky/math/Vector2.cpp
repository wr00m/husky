#include <husky/math/Vector2.hpp>
#include <cmath>

namespace husky {

template<typename T>
Vector2<T>::Vector2()
  : x(0), y(0)
{
}

template<typename T>
Vector2<T>::Vector2(T xy)
  : x(xy), y(xy)
{
}

template<typename T>
Vector2<T>::Vector2(T x, T y)
  : x(x), y(y)
{
}

template<typename T>
void Vector2<T>::set(T x, T y)
{
  this->x = x;
  this->y = y;
}

template<typename T>
void Vector2<T>::normalize()
{
  if (T len = length()) {
    *this /= len;
  }
}

template<typename T>
Vector2<T> Vector2<T>::normalized() const
{
  Vector2<T> res = *this;
  res.normalize();
  return res;
}

template<typename T>
T Vector2<T>::length() const
{
  return (T)std::sqrt(length2());
}

template<typename T>
T Vector2<T>::length2() const
{
  return this->dot(*this);
}

template<typename T>
T Vector2<T>::dot(const Vector2<T> &other) const
{
  return (x * other.x + y * other.y);
}

template<typename T>
Vector2<T> Vector2<T>::lerp(const Vector2<T> &target, T t) const
{
  return{}; // TODO
}

template<typename T>
Vector2<T> Vector2<T>::nlerp(const Vector2<T> &target, T t) const
{
  return{}; // TODO
}

template<typename T>
Vector2<T>& Vector2<T>::operator+=(const Vector2<T> &other)
{
  x += other.x;
  y += other.y;
  return *this;
}

template<typename T>
Vector2<T>& Vector2<T>::operator-=(const Vector2<T> &other)
{
  x -= other.x;
  y -= other.y;
  return *this;
}

template<typename T>
Vector2<T>& Vector2<T>::operator*=(const Vector2<T> &other)
{
  x *= other.x;
  y *= other.y;
  return *this;
}

template<typename T>
Vector2<T>& Vector2<T>::operator/=(const Vector2<T> &other)
{
  x /= other.x;
  y /= other.y;
  return *this;
}

template<typename T>
Vector2<T> Vector2<T>::operator+(const Vector2<T> &other) const
{
  return Vector2<T>(x + other.x, y + other.y);
}

template<typename T>
Vector2<T> Vector2<T>::operator-(const Vector2<T> &other) const
{
  return Vector2<T>(x - other.x, y - other.y);
}

template<typename T>
Vector2<T> Vector2<T>::operator*(const Vector2<T> &other) const
{
  return Vector2<T>(x * other.x, y * other.y);
}

template<typename T>
Vector2<T> Vector2<T>::operator/(const Vector2<T> &other) const
{
  return Vector2<T>(x / other.x, y / other.y);
}

template<typename T>
Vector2<T> Vector2<T>::operator-() const
{
  return Vector2<T>(-x, -y);
}

template class Vector2<double>;
template class Vector2<float>;
template class Vector2<std::uint8_t>;
template class Vector2<std::int32_t>;

}
