#include <husky/math/EulerAngles.hpp>
#include <husky/math/Math.hpp>
#include <cmath>

namespace husky {

template<typename T>
static Vector3<T> rot2Axes(T r11, T r12, T r21, T r31, T r32) // Proper Euler angles
{
  return {
    std::atan2(r31, r32),
    std::acos(Math::clamp(r21, T(-1), T(1))), // Clamp to avoid precision issues
    std::atan2(r11, r12)
  };
}

template<typename T>
static Vector3<T> rot3Axes(T r11, T r12, T r21, T r31, T r32) // Tait-Bryan angles
{
  return {
    std::atan2(r11, r12),                     // Yaw
    std::asin(Math::clamp(r21, T(-1), T(1))), // Pitch: Clamp to avoid precision issues
    std::atan2(r31, r32)                      // Roll
  };
}

template<typename T>
static Vector3<T> quaternionToEuler(RotationOrder rotationOrder, const Quaternion<T> &q)
{
  // http://bediyap.com/programming/convert-quaternion-to-euler-rotations/
  if (rotationOrder == RotationOrder::ZYX) {
    return rot3Axes<T>(
      T( 2) * (q.x*q.y + q.w*q.z),
      q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z,
      T(-2) * (q.x*q.z - q.w*q.y),
      T( 2) * (q.y*q.z + q.w*q.x),
      q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z);
  }
  else if (rotationOrder == RotationOrder::ZYZ) {
    return rot2Axes<T>(
      T( 2) * (q.y*q.z - q.w*q.x),
      T( 2) * (q.x*q.z + q.w*q.y),
      q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z,
      T( 2) * (q.y*q.z + q.w*q.x),
      T(-2) * (q.x*q.z - q.w*q.y));
  }
  else if (rotationOrder == RotationOrder::ZXY) {
    return rot3Axes<T>(
      T(-2) * (q.x*q.y - q.w*q.z),
      q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z,
      T( 2) * (q.y*q.z + q.w*q.x),
      T(-2) * (q.x*q.z - q.w*q.y),
      q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z);
  }
  else if (rotationOrder == RotationOrder::ZXZ) {
    return rot2Axes<T>(
      T( 2) * (q.x*q.z + q.w*q.y),
      T(-2) * (q.y*q.z - q.w*q.x),
      q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z,
      T( 2) * (q.x*q.z - q.w*q.y),
      T( 2) * (q.y*q.z + q.w*q.x));
  }
  else if (rotationOrder == RotationOrder::YXZ) {
    return rot3Axes<T>(
      T( 2) * (q.x*q.z + q.w*q.y),
      q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z,
      T(-2) * (q.y*q.z - q.w*q.x),
      T( 2) * (q.x*q.y + q.w*q.z),
      q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z);
  }
  else if (rotationOrder == RotationOrder::YXY) {
    return rot2Axes<T>(
      T( 2) * (q.x*q.y - q.w*q.z),
      T( 2) * (q.y*q.z + q.w*q.x),
      q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z,
      T( 2) * (q.x*q.y + q.w*q.z),
      T(-2) * (q.y*q.z - q.w*q.x));
  }
  else if (rotationOrder == RotationOrder::YZX) {
    return rot3Axes<T>(
      T(-2) * (q.x*q.z - q.w*q.y),
      q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z,
      T( 2) * (q.x*q.y + q.w*q.z),
      T(-2) * (q.y*q.z - q.w*q.x),
      q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z);
  }
  else if (rotationOrder == RotationOrder::YZY) {
    return rot2Axes<T>(
      T( 2) * (q.y*q.z + q.w*q.x),
      T(-2) * (q.x*q.y - q.w*q.z),
      q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z,
      T( 2) * (q.y*q.z - q.w*q.x),
      T( 2) * (q.x*q.y + q.w*q.z));
  }
  else if (rotationOrder == RotationOrder::XYZ) {
    return rot3Axes<T>(
      T(-2) * (q.y*q.z - q.w*q.x),
      q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z,
      T( 2) * (q.x*q.z + q.w*q.y),
      T(-2) * (q.x*q.y - q.w*q.z),
      q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z);
  }
  else if (rotationOrder == RotationOrder::XYX) {
    return rot2Axes<T>(
      T( 2) * (q.x*q.y + q.w*q.z),
      T(-2) * (q.x*q.z - q.w*q.y),
      q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z,
      T( 2) * (q.x*q.y - q.w*q.z),
      T( 2) * (q.x*q.z + q.w*q.y));
  }
  else if (rotationOrder == RotationOrder::XZY) {
    return rot3Axes<T>(
      T( 2) * (q.y*q.z + q.w*q.x),
      q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z,
      T(-2) * (q.x*q.y - q.w*q.z),
      T( 2) * (q.x*q.z + q.w*q.y),
      q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z);
  }
  else if (rotationOrder == RotationOrder::XZX) {
    return rot2Axes<T>(
      T( 2) * (q.x*q.z - q.w*q.y),
      T( 2) * (q.x*q.y + q.w*q.z),
      q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z,
      T( 2) * (q.x*q.z + q.w*q.y),
      T(-2) * (q.x*q.y - q.w*q.z));
  }
  else {
    return {}; // This shouldn't happen...
  }
}

template<typename T>
EulerAngles<T>::EulerAngles()
  : rotationOrder(RotationOrder::ZXY)
  , angles(0, 0, 0)
{
}

template<typename T>
EulerAngles<T>::EulerAngles(RotationOrder rotationOrder, T yaw, T pitch, T roll)
  : rotationOrder(rotationOrder)
  , angles(yaw, pitch, roll)
{
}

template<typename T>
EulerAngles<T>::EulerAngles(RotationOrder rotationOrder, const Quaternion<T> &q)
  : rotationOrder(rotationOrder)
  , angles(quaternionToEuler(rotationOrder, q.normalized())) // Normalize, just in case
{
}

template<typename T>
EulerAngles<T>::EulerAngles(RotationOrder rotationOrder, const Matrix33<T> &m)
  : EulerAngles(rotationOrder, Quaternion<T>::fromRotationMatrix(m))
{
}

template<typename T>
EulerAngles<T>::EulerAngles(RotationOrder rotationOrder, const Matrix44<T> &m)
  : EulerAngles(rotationOrder, Quaternion<T>::fromRotationMatrix(m.get3x3()))
{
}

template<typename T>
Quaternion<T> EulerAngles<T>::toQuaternion() const
{
  // Note: This could be optimized
  return Quaternion<T>::fromRotationMatrix(toMatrix());
}

static Vector3i getAxisIndices(RotationOrder rotationOrder)
{
  switch (rotationOrder) {
  case RotationOrder::ZYX: return { 2, 1, 0 };
  case RotationOrder::ZYZ: return { 2, 1, 2 };
  case RotationOrder::ZXY: return { 2, 0, 1 };
  case RotationOrder::ZXZ: return { 2, 0, 2 };
  case RotationOrder::YXZ: return { 1, 0, 2 };
  case RotationOrder::YXY: return { 1, 0, 1 };
  case RotationOrder::YZX: return { 1, 2, 0 };
  case RotationOrder::YZY: return { 1, 2, 1 };
  case RotationOrder::XYZ: return { 0, 1, 2 };
  case RotationOrder::XYX: return { 0, 1, 0 };
  case RotationOrder::XZY: return { 0, 2, 1 };
  case RotationOrder::XZX: return { 0, 2, 0 };
  default: return {}; // This shouldn't happen...
  }
}

template<typename T>
Matrix33<T> EulerAngles<T>::toMatrix() const
{
  static const Vector3<T> axes[3] = {
    { 1, 0, 0 },  // X
    { 0, 1, 0 },  // Y
    { 0, 0, 1 },  // Z
  };

  const Vector3i inds = getAxisIndices(rotationOrder);

  // Note: This could be optimized
  return Matrix33<T>::rotate(  yaw, axes[inds[0]])
       * Matrix33<T>::rotate(pitch, axes[inds[1]])
       * Matrix33<T>::rotate( roll, axes[inds[2]]);
}

template<typename T> EulerAngles<T> EulerAngles<T>::operator-() const { return EulerAngles<T>(rotationOrder, -yaw, -pitch, -roll); }

template class EulerAngles<double>;
template class EulerAngles<float>;

}
