#include <husky/math/Quaternion.hpp>
#include <husky/math/Vector4.hpp>
#include <husky/math/Math.hpp>
#include <algorithm>
#include <cmath>

namespace husky {

template<typename T>
Quaternion<T> Quaternion<T>::identity()
{
  return { 0, 0, 0, 1 };
}

template<typename T>
Quaternion<T> Quaternion<T>::fromRotationMatrix(const Matrix33<T> &m)
{
  Vector4<T> tr;
  tr.x = m[0][0] - m[1][1] - m[2][2];
  tr.y = m[1][1] - m[0][0] - m[2][2];
  tr.z = m[2][2] - m[0][0] - m[1][1];
  tr.w = m[0][0] + m[1][1] + m[2][2];

  int iGreatest = 3; // Greatest diagonal element index
  for (int i = 0; i < 3; i++) {
    if (tr[i] > tr[iGreatest]) {
      iGreatest = i;
    }
  }

  T b = std::sqrt(tr[iGreatest] + T(1)) * T(0.5);
  T s = T(0.25) / b;

  switch (iGreatest) {
  case 0:  return {                       b, (m[0][1] + m[1][0]) * s, (m[2][0] + m[0][2]) * s, (m[1][2] - m[2][1]) * s };
  case 1:  return { (m[0][1] + m[1][0]) * s,                       b, (m[1][2] + m[2][1]) * s, (m[2][0] - m[0][2]) * s };
  case 2:  return { (m[2][0] + m[0][2]) * s, (m[1][2] + m[2][1]) * s,                       b, (m[0][1] - m[1][0]) * s };
  case 3:  return { (m[1][2] - m[2][1]) * s, (m[2][0] - m[0][2]) * s, (m[0][1] - m[1][0]) * s,                       b };
  default: return {}; // This shouldn't happen...
  }
}

template<typename T>
Quaternion<T> Quaternion<T>::fromDirections(const Vector3<T> &from, const Vector3<T> &to)
{
  T cosTheta = from.dot(to);
  Vector3<T> axis;
  
  constexpr T tolerance = T(1e-7);
  
  if (cosTheta >= T(1) - tolerance) { // Parallel directions
    return {}; // Identity quaternion
  }
  
  if (cosTheta < T(-1) + tolerance) { // Opposite directions
    axis = Vector3<T>(1, 0, 0).cross(from); // Guess a rotation vector
    if (axis.length2() < tolerance) { // Bad guess!
      axis = Vector3<T>(0, 1, 0).cross(from); // Guess again
    }
    axis.normalize();
    return fromAxisAngle(T(Math::pi), axis);
  }
  
  axis = from.cross(to);
  T s = std::sqrt((T(1) + cosTheta) * T(2));
  
  Quaternion<T> q;
  q.xyz = axis / s;
  q.w = T(0.5) * s;
  return q;
}

template<typename T>
Quaternion<T> Quaternion<T>::fromAxisAngle(T rad, Vector3<T> axis)
{
  //axis.normalize();

  Quaternion<T> q;
  q.xyz = axis * std::sin(rad * T(0.5));
  q.w = std::cos(rad * T(0.5));
  return q;
}

template<typename T>
Quaternion<T>::Quaternion()
  : x(0), y(0), z(0), w(1) // Identity
{
}

template<typename T>
Quaternion<T>::Quaternion(T xyzw)
  : x(xyzw), y(xyzw), z(xyzw), w(xyzw)
{
}

template<typename T>
Quaternion<T>::Quaternion(T x, T y, T z, T w)
  : x(x), y(y), z(z), w(w)
{
}

template<typename T>
Matrix33<T> Quaternion<T>::toMatrix() const
{
  Quaternion<T> n = normalized();
  
  T xx = (n.x * n.x);
  T xy = (n.x * n.y);
  T xz = (n.x * n.z);
  T xw = (n.x * n.w);
  T yy = (n.y * n.y);
  T yz = (n.y * n.z);
  T yw = (n.y * n.w);
  T zz = (n.z * n.z);
  T zw = (n.z * n.w);
  
  Matrix33<T> m;
  //m[0][0] = T(1) - T(2) * (yy + zz);
  //m[0][1] =        T(2) * (xy - zw);
  //m[0][2] =        T(2) * (xz + yw);
  //m[1][0] =        T(2) * (xy + zw);
  //m[1][1] = T(1) - T(2) * (xx + zz);
  //m[1][2] =        T(2) * (yz - xw);
  //m[2][0] =        T(2) * (xz - yw);
  //m[2][1] =        T(2) * (yz + xw);
  //m[2][2] = T(1) - T(2) * (xx + yy);
  m[0][0] = T(1) - T(2) * (yy + zz);
  m[0][1] =        T(2) * (xy + zw);
  m[0][2] =        T(2) * (xz - yw);
  m[1][0] =        T(2) * (xy - zw);
  m[1][1] = T(1) - T(2) * (xx + zz);
  m[1][2] =        T(2) * (yz + xw);
  m[2][0] =        T(2) * (xz + yw);
  m[2][1] =        T(2) * (yz - xw);
  m[2][2] = T(1) - T(2) * (xx + yy);
  return m;
}

template<typename T>
T Quaternion<T>::toAxisAngle(Vector3<T> &axis) const
{
  Quaternion<T> q = *this;
  if (q.w > T(1)) {
    q.normalize();
  }

  axis.set(q.x, q.y, q.z);
  T s = (T)std::sqrt(T(1) - q.w * q.w);

  if (s > 1e-3) { // Avoid divide-by-zero
    axis /= s;
  }

  T angle = T(2) * (T)std::acos(q.w);
  return angle;
}

template<typename T>
T Quaternion<T>::angleAbs(const Quaternion<T> &target) const
{
  Quaternion<T> q = this->conjugated() * target;
  Vector3<T> axis;
  T angle = q.toAxisAngle(axis);
  return angle;
}

template<typename T>
void Quaternion<T>::set(T x, T y, T z, T w)
{
  this->x = x;
  this->y = y;
  this->z = z;
  this->z = w;
}

template<typename T>
void Quaternion<T>::normalize()
{
  if (T len = length()) {
    *this /= len;
  }
}

template<typename T>
void Quaternion<T>::conjugate()
{
  x = -x;
  y = -y;
  z = -z;
}

template<typename T>
Quaternion<T> Quaternion<T>::normalized() const
{
  Quaternion<T> res = *this;
  res.normalize();
  return res;
}

template<typename T>
Quaternion<T> Quaternion<T>::conjugated() const
{
  Quaternion<T> res = *this;
  res.conjugate();
  return res;
}

template<typename T>
T Quaternion<T>::length() const
{
  return (T)std::sqrt(length2());
}

template<typename T>
T Quaternion<T>::length2() const
{
  return this->dot(*this);
}

template<typename T>
T Quaternion<T>::dot(const Quaternion<T> &other) const
{
  return (x * other.x + y * other.y + z * other.z + w * other.w);
}

template<typename T>
Quaternion<T> Quaternion<T>::nlerp(const Quaternion<T> &target, T t) const
{
  return{}; // TODO
}

template<typename T>
Quaternion<T> Quaternion<T>::slerp(const Quaternion<T> &target, T t) const
{
  return{}; // TODO
}

template<typename T>
Vector3<T> Quaternion<T>::operator*(const Vector3<T> &v) const
{
  Vector3<T> t = xyz.cross(v) * T(2);
  return v + (t * w) + xyz.cross(t);
}

template<typename T>
Quaternion<T>& Quaternion<T>::operator*=(const Quaternion<T> &other)
{
  *this = *this * other;
  return *this;
}

template<typename T>
Quaternion<T> Quaternion<T>::operator*(const Quaternion<T> &other) const
{
  Quaternion<T> q;
  q.x =  x * other.w + y * other.z - z * other.y + w * other.x;
  q.y = -x * other.z + y * other.w + z * other.x + w * other.y;
  q.z =  x * other.y - y * other.x + z * other.w + w * other.z;
  q.w = -x * other.x - y * other.y - z * other.z + w * other.w;
  //q.x = w * other.x + x * other.w + y * other.z - z * other.y;
  //q.y = w * other.y + y * other.w + z * other.x - x * other.z;
  //q.z = w * other.z + z * other.w + x * other.y - y * other.x;
  //q.w = w * other.w - x * other.x - y * other.y - z * other.z;
  return q;
}

template<typename T>
Quaternion<T> Quaternion<T>::operator-() const
{
  return Quaternion<T>(-x, -y, -z, -w);
}

template<typename T>
Quaternion<T>& Quaternion<T>::operator+=(T v)
{
  x += v;
  y += v;
  z += v;
  w += v;
  return *this;
}

template<typename T>
Quaternion<T>& Quaternion<T>::operator-=(T v)
{
  x -= v;
  y -= v;
  z -= v;
  w -= v;
  return *this;
}

template<typename T>
Quaternion<T>& Quaternion<T>::operator*=(T v)
{
  x *= v;
  y *= v;
  z *= v;
  w *= v;
  return *this;
}

template<typename T>
Quaternion<T>& Quaternion<T>::operator/=(T v)
{
  x /= v;
  y /= v;
  z /= v;
  w /= v;
  return *this;
}

template<typename T>
Quaternion<T> Quaternion<T>::operator+(T v) const
{
  return Quaternion<T>(x + v, y + v, z + v, w + v);
}

template<typename T>
Quaternion<T> Quaternion<T>::operator-(T v) const
{
  return Quaternion<T>(x - v, y - v, z - v, w - v);
}

template<typename T>
Quaternion<T> Quaternion<T>::operator*(T v) const
{
  return Quaternion<T>(x * v, y * v, z * v, w * v);
}

template<typename T>
Quaternion<T> Quaternion<T>::operator/(T v) const
{
  return Quaternion<T>(x / v, y / v, z / v, w / v);
}

template class Quaternion<double>;
template class Quaternion<float>;

}
