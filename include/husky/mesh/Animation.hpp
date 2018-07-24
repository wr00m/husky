#pragma once

#include <husky/math/Quaternion.hpp>
#include <map>
#include <vector>

namespace husky {

class HUSKY_DLL AnimationChannel
{
public:
  AnimationChannel(const std::string &nodeName);

  std::string nodeName;
  std::map<double, Vector3d> keyframePosition;
  std::map<double, Quaterniond> keyframeRotation;
  std::map<double, Vector3d> keyframeScale;
};

class HUSKY_DLL Animation
{
public:
  Animation(const std::string &name, double durationTicks, double ticksPerSecond);

  std::string name;
  double durationTicks;
  double ticksPerSecond;
  std::vector<AnimationChannel> channels;
};

}
