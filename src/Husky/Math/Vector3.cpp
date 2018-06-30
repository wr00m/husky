#include <Husky/Math/Vector3.hpp>
#include <cmath>

namespace husky {

template<typename T>
Vector3<T>::Vector3()
  : x(0), y(0), z(0)
{
}

template<typename T>
Vector3<T>::Vector3(T xyz)
  : x(xyz), y(xyz), z(xyz)
{
}

template<typename T>
Vector3<T>::Vector3(T x, T y, T z)
  : x(x), y(y), z(z)
{
}

template<typename T>
Vector3<T>::Vector3(const Vector2<T> &xy, T z)
  : x(xy.x), y(xy.y), z(z)
{
}

template<typename T>
void Vector3<T>::set(T x, T y, T z)
{
  this->x = x;
  this->y = y;
  this->z = z;
}

template<typename T>
void Vector3<T>::normalize()
{
  if (T len = length()) {
    *this /= len;
  }
}

template<typename T>
Vector3<T> Vector3<T>::normalized() const
{
  Vector3<T> res = *this;
  res.normalize();
  return res;
}

template<typename T>
T Vector3<T>::length() const
{
  return (T)std::sqrt(length2());
}

template<typename T>
T Vector3<T>::length2() const
{
  return this->dot(*this);
}

template<typename T>
T Vector3<T>::dot(const Vector3<T> &other) const
{
  return (x * other.x + y * other.y + z * other.z);
}

template<typename T>
Vector3<T> Vector3<T>::cross(const Vector3<T> &other) const
{
  return Vector3<T>(
    y * other.z - z * other.y,
    z * other.x - x * other.z,
    x * other.y - y * other.x);
}

template<typename T>
Vector2<T> Vector3<T>::xy() const
{
  return Vector2<T>(x, y);
}

template<typename T>
Vector3<T>& Vector3<T>::operator+=(const Vector3<T> &other)
{
  x += other.x;
  y += other.y;
  z += other.z;
  return *this;
}

template<typename T>
Vector3<T>& Vector3<T>::operator-=(const Vector3<T> &other)
{
  x -= other.x;
  y -= other.y;
  z -= other.z;
  return *this;
}

template<typename T>
Vector3<T>& Vector3<T>::operator*=(const Vector3<T> &other)
{
  x *= other.x;
  y *= other.y;
  z *= other.z;
  return *this;
}

template<typename T>
Vector3<T>& Vector3<T>::operator/=(const Vector3<T> &other)
{
  x /= other.x;
  y /= other.y;
  z /= other.z;
  return *this;
}

template<typename T>
Vector3<T> Vector3<T>::operator+(const Vector3<T> &other) const
{
  return Vector3<T>(x + other.x, y + other.y, z + other.z);
}

template<typename T>
Vector3<T> Vector3<T>::operator-(const Vector3<T> &other) const
{
  return Vector3<T>(x - other.x, y - other.y, z - other.z);
}

template<typename T>
Vector3<T> Vector3<T>::operator*(const Vector3<T> &other) const
{
  return Vector3<T>(x * other.x, y * other.y, z * other.z);
}

template<typename T>
Vector3<T> Vector3<T>::operator/(const Vector3<T> &other) const
{
  return Vector3<T>(x / other.x, y / other.y, z / other.z);
}

template<typename T>
Vector3<T> Vector3<T>::operator-() const
{
  return Vector3<T>(-x, -y, -z);
}

template class Vector3<double>;
template class Vector3<float>;
template class Vector3<std::uint8_t>;
template class Vector3<std::int32_t>;

}
