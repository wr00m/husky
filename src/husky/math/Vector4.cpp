#include <husky/math/Vector4.hpp>
#include <cmath>

namespace husky {

template<typename T>
Vector4<T>::Vector4()
  : x(0), y(0), z(0), w(0)
{
}

template<typename T>
Vector4<T>::Vector4(T xyzw)
  : x(xyzw), y(xyzw), z(xyzw), w(xyzw)
{
}

template<typename T>
Vector4<T>::Vector4(T x, T y, T z, T w)
  : x(x), y(y), z(z), w(w)
{
}

template<typename T>
Vector4<T>::Vector4(const Vector2<T> &xy, T z, T w)
  : x(xy.x), y(xy.y), z(z), w(w)
{
}

template<typename T>
Vector4<T>::Vector4(const Vector3<T> &xyz, T w)
  : x(xyz.x), y(xyz.y), z(xyz.z), w(w)
{
}

template<typename T>
void Vector4<T>::set(T x, T y, T z, T w)
{
  this->x = x;
  this->y = y;
  this->z = z;
  this->z = w;
}

template<typename T>
void Vector4<T>::normalize()
{
  if (T len = length()) {
    *this /= len;
  }
}

template<typename T>
Vector4<T> Vector4<T>::normalized() const
{
  Vector4<T> res = *this;
  res.normalize();
  return res;
}

template<typename T>
T Vector4<T>::length() const
{
  return (T)std::sqrt(length2());
}

template<typename T>
T Vector4<T>::length2() const
{
  return this->dot(*this);
}

template<typename T>
T Vector4<T>::dot(const Vector4<T> &other) const
{
  return (x * other.x + y * other.y + z * other.z + w * other.w);
}

template<typename T>
Vector4<T>& Vector4<T>::operator+=(const Vector4<T> &other)
{
  x += other.x;
  y += other.y;
  z += other.z;
  w += other.w;
  return *this;
}

template<typename T>
Vector4<T>& Vector4<T>::operator-=(const Vector4<T> &other)
{
  x -= other.x;
  y -= other.y;
  z -= other.z;
  w -= other.w;
  return *this;
}

template<typename T>
Vector4<T>& Vector4<T>::operator*=(const Vector4<T> &other)
{
  x *= other.x;
  y *= other.y;
  z *= other.z;
  w *= other.w;
  return *this;
}

template<typename T>
Vector4<T>& Vector4<T>::operator/=(const Vector4<T> &other)
{
  x /= other.x;
  y /= other.y;
  z /= other.z;
  w /= other.w;
  return *this;
}

template<typename T>
Vector4<T> Vector4<T>::operator+(const Vector4<T> &other) const
{
  return Vector4<T>(x + other.x, y + other.y, z + other.z, w + other.w);
}

template<typename T>
Vector4<T> Vector4<T>::operator-(const Vector4<T> &other) const
{
  return Vector4<T>(x - other.x, y - other.y, z - other.z, w - other.w);
}

template<typename T>
Vector4<T> Vector4<T>::operator*(const Vector4<T> &other) const
{
  return Vector4<T>(x * other.x, y * other.y, z * other.z, w * other.w);
}

template<typename T>
Vector4<T> Vector4<T>::operator/(const Vector4<T> &other) const
{
  return Vector4<T>(x / other.x, y / other.y, z / other.z, w / other.w);
}

template<typename T>
Vector4<T> Vector4<T>::operator-() const
{
  return Vector4<T>(-x, -y, -z, -w);
}

template class Vector4<double>;
template class Vector4<float>;
template class Vector4<std::uint8_t>;
template class Vector4<std::int32_t>;

}
