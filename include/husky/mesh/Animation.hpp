#pragma once

#include <husky/math/Matrix44.hpp>
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

class HUSKY_DLL AnimatedNode
{
public:
  AnimatedNode(const std::string &name);

  std::string name;
  bool animated;
  Matrix44d mtxRelToParent;
  Matrix44d mtxRelToModel;
};

class HUSKY_DLL Animation
{
public:
  Animation(const std::string &name, double durationTicks, double ticksPerSecond);

  double getTicks(double seconds) const;
  bool getAnimatedNodeTransform(const std::string &nodeName, double ticks, Matrix44d &mtxAnimNode) const;

  std::string name;
  double durationTicks;
  double ticksPerSecond;
  std::map<std::string, AnimationChannel> channels; // Node name is map key
};

}
