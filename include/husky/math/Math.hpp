#pragma once

namespace husky {

class Math {
public:
  static constexpr double pi = 3.14159265358979323846;
  static constexpr double pi2 = pi / 2.0;
  static constexpr double pi4 = pi / 4.0;
  static constexpr double twoPi = 2.0 * pi;
  static constexpr double deg2rad = pi / 180.0;
  static constexpr double rad2deg = 180.0 / pi;

  template<typename T>
  static T lerp(T v0, T v1, T t)
  {
    return (T(1) - t) * v0 + t * v1;
  }

  template<typename T>
  static T clamp(T v, T min, T max)
  {
    if (v > max) {
      return max;
    }
    else if (v < min) {
      return min;
    }
    else {
      return v;
    }
  }

  template<typename T>
  static T sign(T x)
  {
    if (x > T(0)) {
      return T(1);
    }
    else if (x < T(0)) {
      return T(-1);
    }
    else {
      return T(0);
    }
  }
};

}
