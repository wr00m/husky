#include <husky/math/EulerAngles.hpp>
#include <cmath>

namespace husky {

template<typename T>
static Vector3<T> rot2Axes(T r11, T r12, T r21, T r31, T r32) // Proper Euler angles
{
  return {
    std::atan2(r31, r32),
    std::acos(r21),
    std::atan2(r11, r12)
  };
}

template<typename T>
static Vector3<T> rot3Axes(T r11, T r12, T r21, T r31, T r32) // Tait-Bryan angles
{
  return {
    std::atan2(r11, r12), // Yaw
    std::asin(r21),       // Pitch
    std::atan2(r31, r32)  // Roll
  };
}

template<typename T>
static Vector3<T> quaternionToEuler(RotationOrder rotationOrder, const Quaternion<T> &q)
{
  // http://bediyap.com/programming/convert-quaternion-to-euler-rotations/
  if (rotationOrder == RotationOrder::ZYX) {
    return rot3Axes<T>(2 * (q.x*q.y + q.w*q.z),
      q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z,
      -2 * (q.x*q.z - q.w*q.y),
      2 * (q.y*q.z + q.w*q.x),
      q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z);
  }
  else if (rotationOrder == RotationOrder::ZYZ) {
    return rot2Axes<T>(2 * (q.y*q.z - q.w*q.x),
      2 * (q.x*q.z + q.w*q.y),
      q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z,
      2 * (q.y*q.z + q.w*q.x),
      -2 * (q.x*q.z - q.w*q.y));
  }
  else if (rotationOrder == RotationOrder::ZXY) {
    return rot3Axes<T>(-2 * (q.x*q.y - q.w*q.z),
      q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z,
      2 * (q.y*q.z + q.w*q.x),
      -2 * (q.x*q.z - q.w*q.y),
      q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z);
  }
  else if (rotationOrder == RotationOrder::ZXZ) {
    return rot2Axes<T>(2 * (q.x*q.z + q.w*q.y),
      -2 * (q.y*q.z - q.w*q.x),
      q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z,
      2 * (q.x*q.z - q.w*q.y),
      2 * (q.y*q.z + q.w*q.x));
  }
  else if (rotationOrder == RotationOrder::YXZ) {
    return rot3Axes<T>(2 * (q.x*q.z + q.w*q.y),
      q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z,
      -2 * (q.y*q.z - q.w*q.x),
      2 * (q.x*q.y + q.w*q.z),
      q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z);
  }
  else if (rotationOrder == RotationOrder::YXY) {
    return rot2Axes<T>(2 * (q.x*q.y - q.w*q.z),
      2 * (q.y*q.z + q.w*q.x),
      q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z,
      2 * (q.x*q.y + q.w*q.z),
      -2 * (q.y*q.z - q.w*q.x));
  }
  else if (rotationOrder == RotationOrder::YZX) {
    return rot3Axes<T>(-2 * (q.x*q.z - q.w*q.y),
      q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z,
      2 * (q.x*q.y + q.w*q.z),
      -2 * (q.y*q.z - q.w*q.x),
      q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z);
  }
  else if (rotationOrder == RotationOrder::YZY) {
    return rot2Axes<T>(2 * (q.y*q.z + q.w*q.x),
      -2 * (q.x*q.y - q.w*q.z),
      q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z,
      2 * (q.y*q.z - q.w*q.x),
      2 * (q.x*q.y + q.w*q.z));
  }
  else if (rotationOrder == RotationOrder::XYZ) {
    return rot3Axes<T>(-2 * (q.y*q.z - q.w*q.x),
      q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z,
      2 * (q.x*q.z + q.w*q.y),
      -2 * (q.x*q.y - q.w*q.z),
      q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z);
  }
  else if (rotationOrder == RotationOrder::XYX) {
    return rot2Axes<T>(2 * (q.x*q.y + q.w*q.z),
      -2 * (q.x*q.z - q.w*q.y),
      q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z,
      2 * (q.x*q.y - q.w*q.z),
      2 * (q.x*q.z + q.w*q.y));
  }
  else if (rotationOrder == RotationOrder::XZY) {
    return rot3Axes<T>(2 * (q.y*q.z + q.w*q.x),
      q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z,
      -2 * (q.x*q.y - q.w*q.z),
      2 * (q.x*q.z + q.w*q.y),
      q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z);
  }
  else if (rotationOrder == RotationOrder::XZX) {
    return rot2Axes<T>(2 * (q.x*q.z - q.w*q.y),
      2 * (q.x*q.y + q.w*q.z),
      q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z,
      2 * (q.x*q.z + q.w*q.y),
      -2 * (q.x*q.y - q.w*q.z));
  }
  else {
    return {};
  }
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
  return {}; // TODO
}

template<typename T>
Matrix33<T> EulerAngles<T>::toMatrix() const
{
  return {}; // TODO
}

template class EulerAngles<double>;
template class EulerAngles<float>;

}
