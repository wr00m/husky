#pragma once

namespace husky {

class math {
public:
  static constexpr double pi = 3.14159265358979323846;
  static constexpr double pi2 = pi / 2.0;
  static constexpr double pi4 = pi / 4.0;
  static constexpr double twoPi = 2.0 * pi;
  static constexpr double deg2rad = pi / 180.0;
  static constexpr double rad2deg = 180.0 / pi;

  static double lerp(double v0, double v1, double t)
  {
    return (1 - t) * v0 + t * v1;
  }

  static double clamp(double v, double min, double max)
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
};

}
