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

class HUSKY_DLL Animation
{
public:
  Animation(const std::string &name, double durationTicks, double ticksPerSecond);

  //Matrix44d getBoneTransform(const Bone &bone, double time) const;

  std::string name;
  double durationTicks;
  double ticksPerSecond;
  std::map<std::string, AnimationChannel> channels; // Node name is map key

private:
  Matrix44d getNodeTransform(const std::string &nodeName, double time, const Matrix44d &defaultTransform) const;
};

}
